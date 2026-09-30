// Generated from proto/serein/settings/v1/ghost.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Ghost {

inline constexpr auto kGhostMode = Option<bool>{
	"serein.ghostMode",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_ghost_mode",
	0 };
inline constexpr auto kGhostHideReadReceipts = Option<bool>{
	"serein.ghostHideReadReceipts",
	Scope::Account,
	true,
	Category::Privacy,
	"lng_serein_ghost_hide_read_receipts",
	0 };
inline constexpr auto kGhostHideStoryViews = Option<bool>{
	"serein.ghostHideStoryViews",
	Scope::Account,
	true,
	Category::Privacy,
	"lng_serein_ghost_hide_story_views",
	0 };
inline constexpr auto kGhostHideOnline = Option<bool>{
	"serein.ghostHideOnline",
	Scope::Account,
	true,
	Category::Privacy,
	"lng_serein_ghost_hide_online",
	0 };
inline constexpr auto kGhostHideTyping = Option<bool>{
	"serein.ghostHideTyping",
	Scope::Account,
	true,
	Category::Privacy,
	"lng_serein_ghost_hide_typing",
	0 };
inline constexpr auto kGhostHideViewIncrements = Option<bool>{
	"serein.ghostHideViewIncrements",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_ghost_hide_view_increments",
	0 };
inline constexpr auto kGhostMarkReadAfterSending = Option<bool>{
	"serein.ghostMarkReadAfterSending",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_ghost_mark_read_after_sending",
	0 };
inline constexpr auto kGhostUseScheduledMessages = Option<bool>{
	"serein.ghostUseScheduledMessages",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_ghost_use_scheduled_messages",
	static_cast<unsigned>(Flag::Hidden) };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kGhostMode));
	Expects(registry.Add(kGhostHideReadReceipts));
	Expects(registry.Add(kGhostHideStoryViews));
	Expects(registry.Add(kGhostHideOnline));
	Expects(registry.Add(kGhostHideTyping));
	Expects(registry.Add(kGhostHideViewIncrements));
	Expects(registry.Add(kGhostMarkReadAfterSending));
	Expects(registry.Add(kGhostUseScheduledMessages));
}

} // namespace Serein::Ghost
