#pragma once

#include "serein/core/options.h"

namespace Serein::Chats {

inline constexpr auto kRefreshDialogList = static_cast<unsigned>(
	Flag::RefreshDialogList);

inline constexpr auto kCompactList = Option<bool>{
	"serein.chatListCompact", Scope::Device, false,
	Category::Chats, "lng_serein_compact_chat_list", kRefreshDialogList };
inline constexpr auto kPreviewLines = Option<int>{
	"serein.chatPreviewLines", Scope::Device, 0,
	Category::Chats, "lng_serein_chat_preview_lines", kRefreshDialogList,
	[](const int &value) { return value >= 0 && value <= 3; } };
inline constexpr auto kHideSavedAndArchivedPreviews = Option<bool>{
	"serein.hideSavedAndArchivedPreviews", Scope::Device, false,
	Category::Chats, "lng_serein_hide_saved_and_archived_previews",
	kRefreshDialogList };
inline constexpr auto kHideStories = Option<bool>{
	"serein.hideStories", Scope::Device, false,
	Category::Chats, "lng_serein_hide_stories" };
inline constexpr auto kHideAllChatsFolder = Option<bool>{
	"serein.hideAllChatsFolder", Scope::Device, false,
	Category::Chats, "lng_serein_hide_all_chats_folder" };
inline constexpr auto kShowArchiveInFolders = Option<bool>{
	"serein.showArchiveInFolders", Scope::Device, false,
	Category::Chats, "lng_serein_show_archive_in_folders",
	kRefreshDialogList };
inline constexpr auto kHideFolderUnreadCounters = Option<bool>{
	"serein.hideFolderUnreadCounters", Scope::Device, false,
	Category::Chats, "lng_serein_hide_folder_unread_counters" };
inline constexpr auto kStartupFolderMode = Option<int>{
	"serein.startupFolderMode", Scope::Account, 0,
	Category::Chats, "lng_serein_startup_folder", 0,
	[](const int &value) { return value >= 0 && value <= 2; } };
inline constexpr auto kStartupFolderId = Option<int>{
	"serein.startupFolderId", Scope::Account, 0,
	Category::Chats, "lng_serein_startup_folder", 0,
	[](const int &value) { return value >= 0; } };
inline constexpr auto kLastOpenedFolderId = Option<int>{
	"serein.lastOpenedFolderId", Scope::Account, 0,
	Category::Chats, "lng_serein_startup_folder", 0,
	[](const int &value) { return value >= 0; } };
inline constexpr auto kChatSort = Option<int>{
	"serein.chatSort", Scope::Device, 0,
	Category::Chats, "lng_serein_chat_sort", 0,
	[](const int &value) {
		if (value == 0) return true;
		if (value < 0 || value > 0xFFF) return false;
		auto seen = 0;
		for (auto i = 0; i != 4; ++i) {
			seen |= 1 << ((value >> (4 + i * 2)) & 3);
		}
		return seen == 0xF;
	} };
inline const auto kManagedFolderIds = Option<QString>{
	"serein.managedFolderIds", Scope::Account, QString(),
	Category::Chats, "lng_serein_managed_only", 0,
	[](const QString &value) {
		if (value.isEmpty()) return true;
		auto previous = 0;
		for (const auto &part : value.split(u',')) {
			auto valid = false;
			const auto id = part.toInt(&valid);
			if (!valid || id <= previous || part != QString::number(id)) {
				return false;
			}
			previous = id;
		}
		return true;
	} };
inline constexpr auto kHideSponsoredMessages = Option<bool>{
	"serein.hideSponsoredMessages", Scope::Device, false,
	Category::Chats, "lng_serein_hide_sponsored_messages" };
inline constexpr auto kHideProxySponsor = Option<bool>{
	"serein.hideProxySponsor", Scope::Device, false,
	Category::Chats, "lng_serein_hide_proxy_sponsor" };
inline constexpr auto kHidePremiumPromotions = Option<bool>{
	"serein.hidePremiumPromotions", Scope::Device, false,
	Category::Chats, "lng_serein_hide_premium_promotions" };
inline constexpr auto kHideBirthdaySuggestions = Option<bool>{
	"serein.hideBirthdaySuggestions", Scope::Device, false,
	Category::Chats, "lng_serein_hide_birthday_suggestions" };
inline constexpr auto kDisableScrollToNextChannel = Option<bool>{
	"serein.disableScrollToNextChannel", Scope::Device, false,
	Category::Chats, "lng_serein_disable_scroll_to_next_channel" };
inline constexpr auto kDisableScrollToNextTopic = Option<bool>{
	"serein.disableScrollToNextTopic", Scope::Device, false,
	Category::Chats, "lng_serein_disable_scroll_to_next_topic" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kCompactList));
	Expects(registry.Add(kPreviewLines));
	Expects(registry.Add(kHideSavedAndArchivedPreviews));
	Expects(registry.Add(kHideStories));
	Expects(registry.Add(kHideAllChatsFolder));
	Expects(registry.Add(kShowArchiveInFolders));
	Expects(registry.Add(kHideFolderUnreadCounters));
	Expects(registry.Add(kStartupFolderMode));
	Expects(registry.Add(kStartupFolderId));
	Expects(registry.Add(kLastOpenedFolderId));
	Expects(registry.Add(kChatSort));
	Expects(registry.Add(kManagedFolderIds));
	Expects(registry.Add(kHideSponsoredMessages));
	Expects(registry.Add(kHideProxySponsor));
	Expects(registry.Add(kHidePremiumPromotions));
	Expects(registry.Add(kHideBirthdaySuggestions));
	Expects(registry.Add(kDisableScrollToNextChannel));
	Expects(registry.Add(kDisableScrollToNextTopic));
}

} // namespace Serein::Chats
