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

#include "boxes/about_box.h"
#include "core/click_handler_types.h"
#include "lang/lang_keys.h"
#include "settings/sections/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
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

const auto kMeta = BuildHelper({
	.id = Home::Id(),
	.parentId = MainId(),
	.title = &tr::lng_serein_settings,
	.icon = &st::menuIconSerein,
}, [](SectionBuilder &builder) {
	builder.addButton({
		.title = tr::lng_serein_settings(),
		.icon = { &st::menuIconSerein },
		.label = rpl::single(tr::lng_settings_current_version(
			tr::now, lt_version, currentVersionShortText())),
	});
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
		.icon = { &st::menuIconInfo },
		.onClick = [=] {
			if (controller) {
				ShowLicenses(controller);
			}
		},
	});
	builder.addDivider();
	AddFeatured(builder);
	builder.addDivider();
	builder.addSectionButton({
		.title = tr::lng_serein_interface(),
		.targetSection = InterfaceId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"interface"_q, u"appearance"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_messages(),
		.targetSection = MessagesId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"messages"_q, u"time"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_chats(),
		.targetSection = ChatsId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"chats"_q, u"list"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_compose(),
		.targetSection = ComposeId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"compose"_q, u"send"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_media(),
		.targetSection = MediaId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"media"_q, u"sticker"_q, u"emoji"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_menu(),
		.targetSection = MenuId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"menu"_q, u"actions"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_privacy(),
		.targetSection = PrivacyId(),
		.icon = { &st::menuIconLock },
		.keywords = { u"privacy"_q, u"phone"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_services(),
		.targetSection = ServicesId(),
		.icon = { &st::menuIconTranslate },
		.keywords = { u"translation"_q, u"AI"_q, u"service"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_rules(),
		.targetSection = RulesId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"filter"_q, u"link"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_config_title(),
		.targetSection = ConfigId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"backup"_q, u"import"_q, u"export"_q },
	});
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
