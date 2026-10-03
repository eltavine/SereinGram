#include "serein/settings/home.h"
#include "serein/settings/interface.h"
#include "serein/settings/licenses.h"
#include "serein/settings/messages.h"
#include "serein/settings/menu.h"
#include "serein/settings/chats.h"
#include "serein/settings/compose.h"
#include "serein/settings/config.h"
#include "serein/settings/media.h"
#include "serein/settings/privacy.h"
#include "serein/settings/services.h"
#include "serein/settings/presets.h"
#include "serein/settings/rules.h"
#include "serein/settings/subpages.h"
#include "serein/settings/page.h"
#include "serein/settings/gen/ghost_rows.h"
#include "serein/settings/gen/history_rows.h"
#include "serein/features/updates/checker.h"

#include "boxes/about_box.h"
#include "core/click_handler_types.h"
#include "lang/lang_keys.h"
#include "settings/sections/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class Home final : public Page<Home> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_settings();
	}

	static const SectionBuildMethod kBuild;

};

[[nodiscard]] rpl::producer<QString> StateLabel(rpl::producer<bool> on) {
	return rpl::conditional(
		std::move(on),
		tr::lng_serein_config_on(),
		tr::lng_serein_config_off());
}

void AddFeatured(SectionBuilder &builder) {
	using namespace rpl::mappers;
	const auto controller = builder.controller();
	auto &account = ForAccount(builder.session());
	builder.addButton({
		.id = u"serein/home/ghost"_q,
		.title = (*Ghost::kSubpageTitle)(),
		.icon = { Ghost::kSubpageIcon },
		.label = StateLabel(rpl::combine(
			account.Value(Ghost::kGhostMode),
			ForDevice().Value(Ghost::kGhostAllAccounts)) | rpl::map(_1 || _2)),
		.onClick = [=] {
			if (controller) {
				controller->showSettings(GhostId());
			}
		},
		.keywords = { u"ghost"_q, u"stealth"_q, u"online"_q, u"read"_q },
	});
	builder.addButton({
		.id = u"serein/home/history"_q,
		.title = (*HistorySettings::kSubpageTitle)(),
		.icon = { HistorySettings::kSubpageIcon },
		.label = StateLabel(rpl::combine(
			account.Value(HistorySettings::kHistorySaveDeleted),
			account.Value(HistorySettings::kHistorySaveEdits)
		) | rpl::map(_1 || _2)),
		.onClick = [=] {
			if (controller) {
				controller->showSettings(HistoryId());
			}
		},
		.keywords = { u"deleted"_q, u"edited"_q, u"history"_q },
	});
	builder.addButton({
		.id = u"serein/home/presets"_q,
		.title = tr::lng_serein_presets(),
		.icon = { &st::menuIconCustomize },
		.onClick = [=] {
			if (controller) {
				Presets::ShowPresets(controller);
			}
		},
		.keywords = { u"preset"_q, u"setup"_q, u"quick"_q },
	});
	if (controller) {
		crl::on_main(controller, [=] { Presets::OfferPresetsOnce(controller); });
	}
}

struct PageButton {
	tr::phrase<> title;
	Settings::Type (*id)();
	not_null<const style::icon*> icon;
	QStringList keywords;
};

void AddGroup(
		SectionBuilder &builder,
		rpl::producer<QString> title,
		std::initializer_list<PageButton> pages) {
	builder.addDivider();
	builder.addSkip();
	builder.addSubsectionTitle(std::move(title));
	for (const auto &page : pages) {
		builder.addSectionButton({
			.title = page.title(),
			.targetSection = page.id(),
			.icon = { page.icon },
			.keywords = page.keywords,
		});
	}
	builder.addSkip();
}

void AddVersion(SectionBuilder &builder) {
	using Update = std::optional<Updates::AvailableUpdate>;
	const auto latest = std::make_shared<Update>();
	const auto version = tr::lng_settings_current_version(
		tr::now,
		lt_version,
		currentVersionShortText());
	const auto button = builder.addButton({
		.id = u"serein/home/version"_q,
		.title = tr::lng_serein_settings(),
		.icon = { &st::menuIconSerein },
		.label = Updates::AvailableValue() | rpl::map([=](const Update &update) {
			return update ? tr::lng_serein_update_ready(tr::now) : version;
		}),
		.onClick = [=] {
			UrlClickHandler::Open(!*latest
				? Updates::ReleasesUrl()
				: (*latest)->download.isEmpty()
				? (*latest)->page
				: (*latest)->download);
		},
		.keywords = { u"version"_q, u"changelog"_q, u"release"_q },
	});
	if (button) {
		Updates::AvailableValue(
		) | rpl::on_next([=](const Update &update) {
			*latest = update;
		}, button->lifetime());
	}
}

void AddUpdateCheck(SectionBuilder &builder) {
	if (!Updates::UpdateChecksAvailable()) {
		return;
	}
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/home/check-updates"_q,
		.title = tr::lng_serein_update_check_now(),
		.icon = { &st::menuIconDownload },
		.label = Updates::CheckingValue() | rpl::map([](bool checking) {
			return checking ? tr::lng_serein_update_checking(tr::now) : QString();
		}),
		.onClick = [=] {
			Updates::CheckForUpdatesNow(crl::guard(controller, [=](
					Updates::CheckResult result) {
				if (result == Updates::CheckResult::UpToDate) {
					controller->showToast(tr::lng_serein_update_latest(tr::now));
				} else if (result == Updates::CheckResult::Failed) {
					controller->showToast(tr::lng_serein_update_failed(tr::now));
				}
			}));
		},
		.keywords = { u"update"_q, u"version"_q, u"GitHub"_q },
	});
}

void AddAbout(SectionBuilder &builder) {
	builder.addDivider();
	builder.addSkip();
	builder.addSubsectionTitle(tr::lng_serein_home_about());
	AddVersion(builder);
	AddUpdateCheck(builder);
	builder.addButton({
		.title = tr::lng_serein_source_code(),
		.icon = { &st::menuIconLink },
		.onClick = [] {
			UrlClickHandler::Open(u"https://github.com/eltavine/SereinGram"_q);
		},
	});
	const auto controller = builder.controller();
	builder.addButton({
		.title = tr::lng_serein_licenses(),
		.icon = { &st::menuIconFile },
		.onClick = [=] {
			if (controller) {
				ShowLicenses(controller);
			}
		},
	});
	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = Home::Id(),
	.parentId = MainId(),
	.title = &tr::lng_serein_settings,
	.icon = &st::menuIconSerein,
}, [](SectionBuilder &builder) {
	builder.addSkip();
	AddFeatured(builder);
	builder.addSkip();
	AddGroup(builder, tr::lng_serein_home_appearance(), {
		{ tr::lng_serein_interface, InterfaceId, &st::menuIconPalette,
			{ u"interface"_q, u"appearance"_q } },
		{ tr::lng_serein_messages, MessagesId, &st::menuIconChatBubble,
			{ u"messages"_q, u"time"_q } },
		{ tr::lng_serein_chats, ChatsId, &st::menuIconChats,
			{ u"chats"_q, u"list"_q } },
		{ tr::lng_serein_media, MediaId, &st::menuIconPhoto,
			{ u"media"_q, u"sticker"_q, u"emoji"_q } },
		{ tr::lng_serein_menu, MenuId, &st::menuIconReorder,
			{ u"menu"_q, u"actions"_q } },
	});
	AddGroup(builder, tr::lng_serein_home_tools(), {
		{ tr::lng_serein_compose, ComposeId, &st::menuIconEdit,
			{ u"compose"_q, u"send"_q } },
		{ tr::lng_serein_services, ServicesId, &st::menuIconTranslate,
			{ u"translation"_q, u"AI"_q, u"service"_q } },
		{ tr::lng_serein_rules, RulesId, &st::menuIconTagFilter,
			{ u"filter"_q, u"link"_q } },
	});
	AddGroup(builder, tr::lng_serein_home_privacy(), {
		{ tr::lng_serein_privacy, PrivacyId, &st::menuIconLock,
			{ u"privacy"_q, u"phone"_q } },
		{ tr::lng_serein_config_title, ConfigId, &st::menuIconStorage,
			{ u"backup"_q, u"import"_q, u"export"_q } },
	});
	AddAbout(builder);
	builder.addDividerText(tr::lng_serein_settings_note());
});

const SectionBuildMethod Home::kBuild = kMeta.build;

} // namespace

Settings::Type HomeId() {
	return Home::Id();
}

void AddSettingsEntry(SectionBuilder &builder) {
	builder.addSectionButton({
		.title = tr::lng_serein_settings(),
		.targetSection = Home::Id(),
		.icon = { &st::menuIconSerein },
		.keywords = { u"Serein"_q, u"desktop"_q },
	});
}

} // namespace Serein
