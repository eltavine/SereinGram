#include "serein/settings/config.h"
#include "serein/settings/exchange_preview.h"

#include "serein/core/exchange.h"
#include "serein/hooks/core/language.h"
#include "serein/settings/cloud_backup.h"
#include "serein/settings/chats.h"
#include "serein/settings/compose.h"
#include "serein/settings/home.h"
#include "serein/settings/interface.h"
#include "serein/settings/media.h"
#include "serein/settings/menu.h"
#include "serein/settings/messages.h"
#include "serein/settings/privacy.h"
#include "serein/settings/restart.h"
#include "serein/settings/rules.h"
#include "serein/settings/services.h"
#include "serein/settings/page.h"
#include "serein/settings/rows.h"
#include "serein/display/json_files.h"
#include "core/application.h"
#include "core/version.h"
#include "lang/lang_instance.h"
#include "lang/lang_keys.h"
#include "lang_auto_counts.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtGui/QClipboard>

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

constexpr auto kMaximumImportBytes = 8 * 1024 * 1024;

class ConfigSection final : public Page<ConfigSection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_config_title();
	}

	static const SectionBuildMethod kBuild;

};

[[nodiscard]] ExchangeExport ExportAll(
		not_null<Window::SessionController*> controller) {
	return Exchange::Export(
		ForDevice(),
		AccountOptions(controller),
		RegisteredOptions());
}

Settings::Type CategorySection(Category category) {
	switch (category) {
	case Category::Interface: return InterfaceId();
	case Category::Chats: return ChatsId();
	case Category::Messages: return MessagesId();
	case Category::Compose: return ComposeId();
	case Category::Menu: return MenuId();
	case Category::Media: return MediaId();
	case Category::Privacy: return PrivacyId();
	case Category::Services: return ServicesId();
	case Category::Rules: return RulesId();
	}
	Unexpected("Invalid Serein option category.");
}

void ShowModified(not_null<Window::SessionController*> controller) {
	const auto root = QJsonDocument::fromJson(ExportAll(controller).data).object();
	auto keys = root.value(u"options"_q).toObject().keys();
	keys += root.value(u"account"_q).toObject().keys();
	const auto entries = SearchRegistry::Instance().collectAll(
		&controller->session());
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_config_modified());
		AddSelectableText(box, tr::lng_serein_config_modified_about(tr::now));
		if (keys.isEmpty()) {
			AddSelectableText(box, tr::lng_serein_config_no_changes(tr::now));
		}
		for (const auto &key : keys) {
			const auto encodedKey = key.toUtf8();
			const auto info = RegisteredOptions().Find(std::string_view(
				encodedKey.constData(), encodedKey.size()));
			if (!info) {
				continue;
			}
			const auto title = OptionTitle(*info);
			const auto fallback = CategorySection(info->category);
			const auto found = ranges::find_if(entries, [&](const SearchEntry &entry) {
				return entry.section == fallback && entry.title == title;
			});
			const auto section = (found != entries.end())
				? found->section : fallback;
			const auto id = (found != entries.end()) ? found->id : QString();
			const auto button = box->addRow(object_ptr<Ui::SettingsButton>(
				box, rpl::single(ScopedOptionTitle(*info)), st::settingsButtonNoIcon));
			button->setClickedCallback(crl::guard(controller, [=] {
				box->closeBox();
				if (!id.isEmpty()) {
					controller->setHighlightControlId(id);
				}
				controller->showSettings(section);
			}));
		}
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

void ShowImport(
		not_null<Window::SessionController*> controller,
		const QByteArray &bytes) {
	ShowExchangePreview(
		controller,
		Exchange::PlanImport(
			ForDevice(),
			AccountOptions(controller),
			RegisteredOptions(),
			bytes),
		{
			.title = tr::lng_serein_config_preview,
			.about = tr::lng_serein_config_import_about,
			.apply = tr::lng_serein_config_apply,
			.done = tr::lng_serein_config_imported,
		});
}

void ShowReset(not_null<Window::SessionController*> controller) {
	const auto plan = Exchange::PlanReset(
		ForDevice(),
		AccountOptions(controller),
		RegisteredOptions());
	if (plan.error.isEmpty() && plan.changes.empty()) {
		controller->showToast(tr::lng_serein_config_no_changes(tr::now));
		return;
	}
	ShowExchangePreview(controller, plan, {
		.title = tr::lng_serein_config_reset,
		.about = tr::lng_serein_config_reset_about,
		.apply = tr::lng_serein_config_reset_apply,
		.done = tr::lng_serein_config_reset_done,
	});
}

void Export(not_null<Window::SessionController*> controller) {
	const auto exported = ExportAll(controller);
	if (!exported.invalidKeys.isEmpty()) {
		controller->showToast(tr::lng_serein_config_invalid(tr::now));
		return;
	}
	Display::SaveJsonFile(
		u"serein-settings.json"_q,
		exported.data,
		crl::guard(controller, [=](QString text) {
			controller->showToast(text);
		}));
}

void Import(not_null<Window::SessionController*> controller) {
	Display::OpenJsonFile(
		kMaximumImportBytes,
		crl::guard(controller, [=](QByteArray bytes) {
			ShowImport(controller, bytes);
		}),
		crl::guard(controller, [=](QString text) {
			controller->showToast(text);
		}));
}

void BackUp(not_null<Window::SessionController*> controller) {
	const auto exported = ExportAll(controller);
	if (!exported.invalidKeys.isEmpty()) {
		controller->showToast(tr::lng_serein_config_invalid(tr::now));
		return;
	}
	BackUpToSavedMessages(controller, exported.data);
}

void Restore(not_null<Window::SessionController*> controller) {
	RestoreFromSavedMessages(
		controller,
		crl::guard(controller, [=](QByteArray bytes) {
			ShowImport(controller, bytes);
		}));
}

void CopyDiagnostics(not_null<Window::SessionController*> controller) {
	const auto &registry = RegisteredOptions();
	const auto exported = Exchange::Export(ForDevice(), registry);
	const auto modified = QJsonDocument::fromJson(exported.data
		).object().value(u"options"_q).toObject().size();
	const auto managed = ranges::count_if(registry.All(), [](const OptionInfo &info) {
		return info.scope == Scope::Device;
	});
	const auto report = tr::lng_serein_config_diagnostic_report(
		tr::now,
		lt_version, QString::fromLatin1(AppVersionStr),
		lt_amount, QString::number(managed),
		lt_value, QString::number(modified),
		lt_error, QString::number(exported.invalidKeys.size()));
	QGuiApplication::clipboard()->setText(report);
	controller->showToast(tr::lng_serein_config_copy_report(tr::now));
}

constexpr auto kTitle = &tr::lng_serein_config_title;
const auto kIcon = &st::menuIconStorage;

const auto kMeta = BuildHelper({
	.id = ConfigSection::Id(),
	.parentId = HomeId(),
	.title = kTitle,
	.icon = kIcon,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	AddSection(builder, {
		u"serein/config/files"_q,
		tr::lng_serein_section_config_files,
		{ u"export"_q, u"import"_q },
	});
	AddRow(builder, {
		.id = u"serein/config/export"_q,
		.title = tr::lng_serein_config_export(),
		.onClick = [=] { Export(controller); },
		.visual = {
			.icon = &st::menuIconExportTheme,
			.about = tr::lng_serein_config_export_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/config/import"_q,
		.title = tr::lng_serein_config_import(),
		.onClick = [=] { Import(controller); },
		.visual = {
			.icon = &st::menuIconImportTheme,
			.about = tr::lng_serein_config_import_row_about,
		},
	});
	EndSection(builder, tr::lng_serein_config_scope);
	AddSection(builder, {
		u"serein/config/saved-backup"_q,
		tr::lng_serein_section_config_backup,
		{ u"backup"_q, u"restore"_q },
	});
	AddRow(builder, {
		.id = u"serein/config/backup"_q,
		.title = tr::lng_serein_config_backup(),
		.onClick = [=] { BackUp(controller); },
		.visual = {
			.icon = &st::menuIconSavedMessages,
			.about = tr::lng_serein_config_backup_row_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/config/restore"_q,
		.title = tr::lng_serein_config_restore(),
		.onClick = [=] { Restore(controller); },
		.visual = {
			.icon = &st::menuIconRestore,
			.about = tr::lng_serein_config_restore_about,
		},
	});
	EndSection(builder, tr::lng_serein_config_backup_about);
	AddSection(builder, {
		u"serein/config/review"_q,
		tr::lng_serein_section_config_review,
		{ u"modified"_q, u"reset"_q, u"diagnostics"_q },
	});
	AddRow(builder, {
		.id = u"serein/config/modified"_q,
		.title = tr::lng_serein_config_modified(),
		.onClick = [=] { ShowModified(controller); },
		.visual = {
			.icon = &st::menuIconEdit,
			.about = tr::lng_serein_config_modified_row_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/config/reset"_q,
		.title = tr::lng_serein_config_reset(),
		.onClick = [=] { ShowReset(controller); },
		.visual = {
			.icon = &st::menuIconRetractVote,
			.about = tr::lng_serein_config_reset_row_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/config/diagnostics"_q,
		.title = tr::lng_serein_config_diagnostics(),
		.onClick = [=] { CopyDiagnostics(controller); },
		.visual = {
			.icon = &st::menuIconCopy,
			.about = tr::lng_serein_config_diagnostics_about,
		},
	});
	EndSection(builder);
});

const SectionBuildMethod ConfigSection::kBuild = kMeta.build;

} // namespace

Settings::Type ConfigId() {
	return ConfigSection::Id();
}

void AddConfigButton(SectionBuilder &builder) {
	builder.addSectionButton({
		.title = (*kTitle)(),
		.targetSection = ConfigSection::Id(),
		.icon = { kIcon },
		.keywords = { u"backup"_q, u"import"_q, u"export"_q },
	});
}

} // namespace Serein
