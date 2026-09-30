// Generated from proto/serein/settings/v1/ghost.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Ghost {

inline const auto kToggleRows = std::array<ToggleRow, 8>{ {
	{
		&kGhostMode,
		tr::lng_serein_ghost_mode,
		u"serein/ghost/ghost-mode"_q,
		{  },
	},
	{
		&kGhostHideReadReceipts,
		tr::lng_serein_ghost_hide_read_receipts,
		u"serein/ghost/ghost-hide-read-receipts"_q,
		{  },
	},
	{
		&kGhostHideStoryViews,
		tr::lng_serein_ghost_hide_story_views,
		u"serein/ghost/ghost-hide-story-views"_q,
		{  },
	},
	{
		&kGhostHideOnline,
		tr::lng_serein_ghost_hide_online,
		u"serein/ghost/ghost-hide-online"_q,
		{  },
	},
	{
		&kGhostHideTyping,
		tr::lng_serein_ghost_hide_typing,
		u"serein/ghost/ghost-hide-typing"_q,
		{  },
	},
	{
		&kGhostHideViewIncrements,
		tr::lng_serein_ghost_hide_view_increments,
		u"serein/ghost/ghost-hide-view-increments"_q,
		{  },
	},
	{
		&kGhostMarkReadAfterSending,
		tr::lng_serein_ghost_mark_read_after_sending,
		u"serein/ghost/ghost-mark-read-after-sending"_q,
		{  },
	},
	{
		&kGhostUseScheduledMessages,
		tr::lng_serein_ghost_use_scheduled_messages,
		u"serein/ghost/ghost-use-scheduled-messages"_q,
		{  },
	},
} };

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder) {
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	AddToggle(builder, kToggleRows[7]);
}

} // namespace Serein::Ghost
