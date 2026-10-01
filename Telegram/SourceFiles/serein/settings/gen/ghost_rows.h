// Generated from proto/serein/settings/v1/ghost.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"

#include <array>

namespace Serein::Ghost {

inline const auto kToggleRows = std::array<ToggleRow, 11>{ {
	{
		&kGhostMode,
		tr::lng_serein_ghost_mode,
		u"serein/ghost/ghost-mode"_q,
		{ u"ghost"_q, u"online"_q, u"typing"_q },
	},
	{
		&kGhostAllAccounts,
		tr::lng_serein_ghost_all_accounts,
		u"serein/ghost/ghost-all-accounts"_q,
		{ u"ghost"_q, u"all accounts"_q, u"global"_q },
	},
	{
		&kGhostHideReadReceipts,
		tr::lng_serein_ghost_hide_read_receipts,
		u"serein/ghost/ghost-hide-read-receipts"_q,
		{ u"ghost"_q, u"read"_q, u"receipts"_q },
	},
	{
		&kGhostHideStoryViews,
		tr::lng_serein_ghost_hide_story_views,
		u"serein/ghost/ghost-hide-story-views"_q,
		{ u"ghost"_q, u"stories"_q },
	},
	{
		&kGhostHideOnline,
		tr::lng_serein_ghost_hide_online,
		u"serein/ghost/ghost-hide-online"_q,
		{ u"ghost"_q, u"online"_q },
	},
	{
		&kGhostHideTyping,
		tr::lng_serein_ghost_hide_typing,
		u"serein/ghost/ghost-hide-typing"_q,
		{ u"ghost"_q, u"typing"_q },
	},
	{
		&kGhostHideViewIncrements,
		tr::lng_serein_ghost_hide_view_increments,
		u"serein/ghost/ghost-hide-view-increments"_q,
		{ u"ghost"_q, u"views"_q },
	},
	{
		&kGhostMarkReadAfterSending,
		tr::lng_serein_ghost_mark_read_after_sending,
		u"serein/ghost/ghost-mark-read-after-sending"_q,
		{ u"ghost"_q, u"read"_q, u"send"_q },
	},
	{
		&kGhostExplicitReadReceipts,
		tr::lng_serein_ghost_explicit_read_receipts,
		u"serein/ghost/ghost-explicit-read-receipts"_q,
		{ u"ghost"_q, u"read"_q, u"mark as read"_q },
	},
	{
		&kGhostSendSilently,
		tr::lng_serein_ghost_send_silently,
		u"serein/ghost/ghost-send-silently"_q,
		{ u"ghost"_q, u"silent"_q, u"notification"_q, u"send"_q },
	},
	{
		&kGhostUseScheduledMessages,
		tr::lng_serein_ghost_use_scheduled_messages,
		u"serein/ghost/ghost-use-scheduled-messages"_q,
		{ u"ghost"_q, u"scheduled"_q, u"send"_q },
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
	AddNote(builder, tr::lng_serein_ghost_all_accounts_note);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddNote(builder, tr::lng_serein_ghost_note);
	custom.readReceiptExceptions();
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_ghost_mode;
inline const auto kSubpageIcon = &st::menuIconStealth;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	builder.addSectionButton({
		.title = (*kSubpageTitle)(),
		.targetSection = section,
		.icon = { kSubpageIcon },
		.keywords = { u"ghost"_q, u"stealth"_q, u"online"_q, u"read"_q },
	});
}

} // namespace Serein::Ghost
