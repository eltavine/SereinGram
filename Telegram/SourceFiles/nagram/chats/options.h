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
inline constexpr auto kHideAllChatsFolder = Option<bool>{
	"nagram.hideAllChatsFolder", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_all_chats_folder" };
inline constexpr auto kShowArchiveInFolders = Option<bool>{
	"nagram.showArchiveInFolders", Scope::Device, false,
	Category::Chats, "lng_nagram_show_archive_in_folders",
	kRefreshDialogList };
inline constexpr auto kHideFolderUnreadCounters = Option<bool>{
	"nagram.hideFolderUnreadCounters", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_folder_unread_counters" };
inline constexpr auto kHideSponsoredMessages = Option<bool>{
	"nagram.hideSponsoredMessages", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_sponsored_messages" };
inline constexpr auto kHideProxySponsor = Option<bool>{
	"nagram.hideProxySponsor", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_proxy_sponsor" };
inline constexpr auto kHidePremiumPromotions = Option<bool>{
	"nagram.hidePremiumPromotions", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_premium_promotions" };
inline constexpr auto kHideBirthdaySuggestions = Option<bool>{
	"nagram.hideBirthdaySuggestions", Scope::Device, false,
	Category::Chats, "lng_nagram_hide_birthday_suggestions" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kCompactList));
	Expects(registry.Add(kPreviewLines));
	Expects(registry.Add(kHideSavedAndArchivedPreviews));
	Expects(registry.Add(kHideStories));
	Expects(registry.Add(kHideAllChatsFolder));
	Expects(registry.Add(kShowArchiveInFolders));
	Expects(registry.Add(kHideFolderUnreadCounters));
	Expects(registry.Add(kHideSponsoredMessages));
	Expects(registry.Add(kHideProxySponsor));
	Expects(registry.Add(kHidePremiumPromotions));
	Expects(registry.Add(kHideBirthdaySuggestions));
}

} // namespace Nagram::Chats
