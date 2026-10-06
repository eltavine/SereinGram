// Generated from proto/serein/settings/v1/ghost.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Ghost {

inline const auto kToggleRows = std::array<ToggleRow, 11>{ {
	{
		.option = &kGhostMode,
		.title = tr::lng_serein_ghost_mode,
		.id = u"serein/ghost/ghost-mode"_q,
		.keywords = { u"ghost"_q, u"online"_q, u"typing"_q },
		.icon = &st::menuIconStealthLocked,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_mode_about,
	},
	{
		.option = &kGhostAllAccounts,
		.title = tr::lng_serein_ghost_all_accounts,
		.id = u"serein/ghost/ghost-all-accounts"_q,
		.keywords = { u"ghost"_q, u"all accounts"_q, u"global"_q },
		.icon = &st::menuIconGroups,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_all_accounts_about,
	},
	{
		.option = &kGhostHideReadReceipts,
		.title = tr::lng_serein_ghost_hide_read_receipts,
		.id = u"serein/ghost/ghost-hide-read-receipts"_q,
		.keywords = { u"ghost"_q, u"read"_q, u"receipts"_q },
		.icon = &st::menuIconMarkUnread,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_hide_read_receipts_about,
	},
	{
		.option = &kGhostHideStoryViews,
		.title = tr::lng_serein_ghost_hide_story_views,
		.id = u"serein/ghost/ghost-hide-story-views"_q,
		.keywords = { u"ghost"_q, u"stories"_q },
		.icon = &st::menuIconStoriesSavedSection,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_hide_story_views_about,
	},
	{
		.option = &kGhostHideOnline,
		.title = tr::lng_serein_ghost_hide_online,
		.id = u"serein/ghost/ghost-hide-online"_q,
		.keywords = { u"ghost"_q, u"online"_q },
		.icon = &st::menuIconUserHide,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_hide_online_about,
	},
	{
		.option = &kGhostHideTyping,
		.title = tr::lng_serein_ghost_hide_typing,
		.id = u"serein/ghost/ghost-hide-typing"_q,
		.keywords = { u"ghost"_q, u"typing"_q },
		.icon = &st::menuIconShortcut,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_hide_typing_about,
	},
	{
		.option = &kGhostHideViewIncrements,
		.title = tr::lng_serein_ghost_hide_view_increments,
		.id = u"serein/ghost/ghost-hide-view-increments"_q,
		.keywords = { u"ghost"_q, u"views"_q },
		.icon = &st::menuIconRepeat,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_ghost_hide_view_increments_about,
	},
	{
		.option = &kGhostMarkReadAfterSending,
		.title = tr::lng_serein_ghost_mark_read_after_sending,
		.id = u"serein/ghost/ghost-mark-read-after-sending"_q,
		.keywords = { u"ghost"_q, u"read"_q, u"send"_q },
		.icon = &st::menuIconSend,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_ghost_mark_read_after_sending_about,
	},
	{
		.option = &kGhostExplicitReadReceipts,
		.title = tr::lng_serein_ghost_explicit_read_receipts,
		.id = u"serein/ghost/ghost-explicit-read-receipts"_q,
		.keywords = { u"ghost"_q, u"read"_q, u"mark as read"_q },
		.icon = &st::menuIconMarkRead,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_ghost_explicit_read_receipts_about,
	},
	{
		.option = &kGhostSendSilently,
		.title = tr::lng_serein_ghost_send_silently,
		.id = u"serein/ghost/ghost-send-silently"_q,
		.keywords = { u"ghost"_q, u"silent"_q, u"notification"_q, u"send"_q },
		.icon = &st::menuIconSilent,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_ghost_send_silently_about,
	},
	{
		.option = &kGhostUseScheduledMessages,
		.title = tr::lng_serein_ghost_use_scheduled_messages,
		.id = u"serein/ghost/ghost-use-scheduled-messages"_q,
		.keywords = { u"ghost"_q, u"scheduled"_q, u"send"_q },
		.icon = &st::menuIconSchedule,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_ghost_use_scheduled_messages_about,
	},
} };

struct CustomRows {
	CustomRow readReceiptExceptions;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	EndSection(builder, tr::lng_serein_ghost_all_accounts_note);
	AddSection(builder, {
		u"serein/ghost/hide"_q,
		tr::lng_serein_section_ghost_hide,
		{ u"hide"_q, u"online"_q, u"typing"_q },
	});
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	EndSection(builder, tr::lng_serein_section_ghost_hide_note);
	AddSection(builder, {
		u"serein/ghost/behavior"_q,
		tr::lng_serein_section_ghost_behavior,
		{ u"read"_q, u"send"_q, u"scheduled"_q },
	});
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	custom.readReceiptExceptions();
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	EndSection(builder, tr::lng_serein_ghost_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_ghost_mode;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_ghost_about;
inline const auto kSubpageIcon = &st::menuIconStealth;
inline const auto kSubpageTile = &st::settingsIconBg3;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"ghost"_q, u"stealth"_q, u"online"_q, u"read"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::Ghost
