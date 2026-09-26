#pragma once

#include "nagram/core/options.h"

namespace Nagram::Chats {

inline constexpr auto kRefreshDialogList = static_cast<unsigned>(
	Flag::RefreshDialogList);

inline constexpr auto kCompactList = Option<bool>{
	"nagram.chatListCompact", Scope::Device, false,
	Category::Chats, "lng_nagram_compact_chat_list", kRefreshDialogList };
inline constexpr auto kPreviewLines = Option<int>{
	"nagram.chatPreviewLines", Scope::Device, 0,
	Category::Chats, "lng_nagram_chat_preview_lines", kRefreshDialogList,
	[](const int &value) { return value >= 0 && value <= 3; } };
inline constexpr auto kHideSavedAndArchivedPreviews = Option<bool>{
	"nagram.hideSavedAndArchivedPreviews", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_saved_and_archived_previews",
	kRefreshDialogList };
inline constexpr auto kHideStories = Option<bool>{
	"nagram.hideStories", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_stories" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kCompactList));
	Expects(registry.Add(kPreviewLines));
	Expects(registry.Add(kHideSavedAndArchivedPreviews));
	Expects(registry.Add(kHideStories));
}

} // namespace Nagram::Chats
