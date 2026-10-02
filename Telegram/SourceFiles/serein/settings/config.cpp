#include "serein/settings/config.h"

#include "serein/core/exchange.h"
#include "serein/hooks/core/language.h"
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
#include "serein/settings/lock.h"
#include "core/application.h"
#include "core/file_utilities.h"
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

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>
#include <QtGui/QClipboard>

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

constexpr auto kMaximumImportBytes = 8 * 1024 * 1024;

class ConfigSection final : public Section<ConfigSection> {
public:
	ConfigSection(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		Serein::GuardSettings(content, [=] { build(content, kBuild); });
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_config_title();
	}

	static const SectionBuildMethod kBuild;

};

QString Title(const OptionInfo &info) {
	const auto key = QByteArray(info.titleKey.data(), info.titleKey.size());
	const auto index = Lang::GetKeyIndex(QLatin1String(key));
	return (index == Lang::kKeysCount)
		? QString::fromUtf8(key)
		: LocalizedValue(Lang::GetInstance(), index);
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

void AddText(not_null<Ui::GenericBox*> box, const QString &text) {
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box, text, st::boxLabel));
	label->setSelectable(true);
	label->setBreakEverywhere(true);
}

QString ValueText(const OptionInfo &info, const QByteArray &raw) {
	const auto value = raw.isEmpty() ? info.fallbackRaw : raw;
	if (info.type == OptionInfo::ValueType::Boolean) {
		return (value == "1")
			? tr::lng_serein_config_on(tr::now)
			: tr::lng_serein_config_off(tr::now);
	}
	auto result = (info.type == OptionInfo::ValueType::String)
		? QString::fromUtf8(value.mid(1))
		: QString::fromUtf8(value);
	constexpr auto kPreviewLength = 120;
	if (result.size() > kPreviewLength) {
		result.truncate(kPreviewLength);
		if (result.back().isHighSurrogate()) {
			result.chop(1);
		}
		result += QChar(0x2026);
	}
	return result;
}

void ShowModified(not_null<Window::SessionController*> controller) {
	const auto exported = Exchange::Export(ForDevice(), RegisteredOptions());
	const auto values = QJsonDocument::fromJson(exported.data
		).object().value(u"options"_q).toObject();
	const auto entries = SearchRegistry::Instance().collectAll(
		&controller->session());
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_config_modified());
		AddText(box, tr::lng_serein_config_modified_about(tr::now));
		if (values.isEmpty()) {
			AddText(box, tr::lng_serein_config_no_changes(tr::now));
		}
		for (auto i = values.begin(); i != values.end(); ++i) {
			const auto encodedKey = i.key().toUtf8();
			const auto info = RegisteredOptions().Find(std::string_view(
				encodedKey.constData(), encodedKey.size()));
			if (!info) {
				continue;
			}
			const auto title = Title(*info);
			const auto fallback = CategorySection(info->category);
			const auto found = ranges::find_if(entries, [&](const SearchEntry &entry) {
				return entry.section == fallback && entry.title == title;
			});
			const auto section = (found != entries.end())
				? found->section : fallback;
			const auto id = (found != entries.end()) ? found->id : QString();
			const auto button = box->addRow(object_ptr<Ui::SettingsButton>(
				box, rpl::single(title), st::settingsButtonNoIcon));
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

struct PlanTexts {
	tr::phrase<> title;
	tr::phrase<> about;
	tr::phrase<> apply;
	tr::phrase<> done;
};

void ShowPlan(
		not_null<Window::SessionController*> controller,
		const ExchangePlan &plan,
		const PlanTexts &texts) {
	if (!plan.error.isEmpty()) {
		controller->showToast(plan.error);
		return;
	}
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(texts.title());
		AddText(box, texts.about(tr::now));
		AddText(box, tr::lng_serein_config_changes(
			tr::now, lt_amount, QString::number(plan.changes.size())));
		for (const auto &change : plan.changes) {
			const auto encodedKey = change.key.toUtf8();
			const auto info = RegisteredOptions().Find(std::string_view(
				encodedKey.constData(), encodedKey.size()));
			if (info) {
				AddText(box, Title(*info) + u"\n"_q
					+ ValueText(*info, change.before)
					+ u" → "_q + ValueText(*info, change.after));
			}
		}
		if (!plan.skippedKeys.isEmpty()) {
			AddText(box, tr::lng_serein_config_unknown(
				tr::now, lt_amount, QString::number(plan.skippedKeys.size()))
				+ u"\n"_q + plan.skippedKeys.mid(0, 20).join('\n'));
		}
		if (!plan.changes.empty()) {
			box->addButton(texts.apply(),
				crl::guard(controller, [=] {
					const auto result = Exchange::Apply(
						ForDevice(), RegisteredOptions(), plan);
					if (!result.applied) {
						box->showToast(result.error);
						return;
					}
					box->closeBox();
					controller->showToast(texts.done(tr::now));
					if (ranges::any_of(plan.changes, [](const ExchangeChange &change) {
						const auto key = change.key.toUtf8();
						const auto info = RegisteredOptions().Find(std::string_view(
							key.constData(), key.size()));
						return info && (info->flags
							& static_cast<unsigned>(Flag::RequiresRestart));
					})) {
						ShowRestartPrompt(controller);
					}
				}));
		}
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

void ShowImport(
		not_null<Window::SessionController*> controller,
		const QByteArray &bytes) {
	ShowPlan(
		controller,
		Exchange::PlanImport(ForDevice(), RegisteredOptions(), bytes),
		{
			.title = tr::lng_serein_config_preview,
			.about = tr::lng_serein_config_import_about,
			.apply = tr::lng_serein_config_apply,
			.done = tr::lng_serein_config_imported,
		});
}

void ShowReset(not_null<Window::SessionController*> controller) {
	const auto plan = Exchange::PlanReset(ForDevice(), RegisteredOptions());
	if (plan.error.isEmpty() && plan.changes.empty()) {
		controller->showToast(tr::lng_serein_config_no_changes(tr::now));
		return;
	}
	ShowPlan(controller, plan, {
		.title = tr::lng_serein_config_reset,
		.about = tr::lng_serein_config_reset_about,
		.apply = tr::lng_serein_config_reset_apply,
		.done = tr::lng_serein_config_reset_done,
	});
}

void Export(not_null<Window::SessionController*> controller) {
	const auto exported = Exchange::Export(ForDevice(), RegisteredOptions());
	if (!exported.invalidKeys.isEmpty()) {
		controller->showToast(tr::lng_serein_config_invalid(tr::now));
		return;
	}
	FileDialog::GetWritePath(
		Core::App().getFileDialogParent(),
		tr::lng_serein_config_export(tr::now),
		tr::lng_serein_config_file_filter(tr::now),
		u"serein-settings.json"_q,
		crl::guard(controller, [=](QString &&path) {
			if (path.isEmpty()) {
				return;
			}
			auto file = QSaveFile(path);
			if (!file.open(QIODevice::WriteOnly)
				|| file.write(exported.data) != exported.data.size()
				|| !file.commit()) {
				controller->showToast(tr::lng_serein_config_write_error(tr::now));
			} else {
				controller->showToast(tr::lng_serein_config_exported(tr::now));
			}
		}));
}

void Import(not_null<Window::SessionController*> controller) {
	FileDialog::GetOpenPath(
		Core::App().getFileDialogParent(),
		tr::lng_serein_config_import(tr::now),
		tr::lng_serein_config_file_filter(tr::now),
		crl::guard(controller, [=](FileDialog::OpenResult &&result) {
			if (!result.paths.isEmpty()) {
				auto file = QFile(result.paths.front());
				if (!file.open(QIODevice::ReadOnly)) {
					controller->showToast(tr::lng_serein_config_read_error(tr::now));
					return;
				}
				ShowImport(controller, file.read(kMaximumImportBytes + 1));
			} else if (!result.remoteContent.isEmpty()) {
				ShowImport(controller, result.remoteContent);
			}
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

const auto kMeta = BuildHelper({
	.id = ConfigSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_config_title,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	builder.addDividerText(tr::lng_serein_config_scope());
	builder.addButton({
		.id = u"serein/config/modified"_q,
		.title = tr::lng_serein_config_modified(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { ShowModified(controller); },
	});
	builder.addButton({
		.id = u"serein/config/export"_q,
		.title = tr::lng_serein_config_export(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { Export(controller); },
	});
	builder.addButton({
		.id = u"serein/config/import"_q,
		.title = tr::lng_serein_config_import(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { Import(controller); },
	});
	builder.addButton({
		.id = u"serein/config/reset"_q,
		.title = tr::lng_serein_config_reset(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { ShowReset(controller); },
	});
	builder.addButton({
		.id = u"serein/config/diagnostics"_q,
		.title = tr::lng_serein_config_diagnostics(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { CopyDiagnostics(controller); },
	});
});

const SectionBuildMethod ConfigSection::kBuild = kMeta.build;

} // namespace

Settings::Type ConfigId() {
	return ConfigSection::Id();
}

} // namespace Serein
