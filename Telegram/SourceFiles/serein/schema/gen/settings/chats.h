// Generated from proto/serein/settings/v1/chats.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Chats {

[[nodiscard]] bool ValidChatSort(const int &value);
[[nodiscard]] bool ValidManagedFolderIds(const QString &value);
[[nodiscard]] bool ValidRecentChats(const QString &value);
[[nodiscard]] bool ValidReadingPositions(const QString &value);

inline constexpr auto kCompactList = Option<bool>{
	"serein.chatListCompact",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_compact_chat_list",
	static_cast<unsigned>(Flag::RefreshDialogList) };
inline constexpr auto kPreviewLines = Option<int>{
	"serein.chatPreviewLines",
	Scope::Device,
	0,
	Category::Chats,
	"lng_serein_chat_preview_lines",
	static_cast<unsigned>(Flag::RefreshDialogList),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 3));
	} };
inline constexpr auto kHideSavedAndArchivedPreviews = Option<bool>{
	"serein.hideSavedAndArchivedPreviews",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_saved_and_archived_previews",
	static_cast<unsigned>(Flag::RefreshDialogList) };
inline constexpr auto kHideStories = Option<bool>{
	"serein.hideStories",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_stories",
	0 };
inline constexpr auto kStartupFolderMode = Option<int>{
	"serein.startupFolderMode",
	Scope::Account,
	0,
	Category::Chats,
	"lng_serein_startup_folder",
	0,
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 2));
	} };
inline constexpr auto kStartupFolderId = Option<int>{
	"serein.startupFolderId",
	Scope::Account,
	0,
	Category::Chats,
	"lng_serein_startup_folder",
	static_cast<unsigned>(Flag::Hidden),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0));
	} };
inline constexpr auto kLastOpenedFolderId = Option<int>{
	"serein.lastOpenedFolderId",
	Scope::Account,
	0,
	Category::Chats,
	"lng_serein_startup_folder",
	static_cast<unsigned>(Flag::Hidden),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0));
	} };
inline constexpr auto kHideAllChatsFolder = Option<bool>{
	"serein.hideAllChatsFolder",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_all_chats_folder",
	0 };
inline constexpr auto kShowArchiveInFolders = Option<bool>{
	"serein.showArchiveInFolders",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_show_archive_in_folders",
	static_cast<unsigned>(Flag::RefreshDialogList) };
inline constexpr auto kHideFolderUnreadCounters = Option<bool>{
	"serein.hideFolderUnreadCounters",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_folder_unread_counters",
	0 };
inline constexpr auto kChatSort = Option<int>{
	"serein.chatSort",
	Scope::Device,
	0,
	Category::Chats,
	"lng_serein_chat_sort",
	0,
	&ValidChatSort };
inline const auto kManagedFolderIds = Option<QString>{
	"serein.managedFolderIds",
	Scope::Account,
	QString(),
	Category::Chats,
	"lng_serein_managed_only",
	static_cast<unsigned>(Flag::Hidden),
	&ValidManagedFolderIds };
inline constexpr auto kHideSponsoredMessages = Option<bool>{
	"serein.hideSponsoredMessages",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_sponsored_messages",
	0 };
inline constexpr auto kHideProxySponsor = Option<bool>{
	"serein.hideProxySponsor",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_proxy_sponsor",
	0 };
inline constexpr auto kHidePremiumPromotions = Option<bool>{
	"serein.hidePremiumPromotions",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_premium_promotions",
	0 };
inline constexpr auto kHideBirthdaySuggestions = Option<bool>{
	"serein.hideBirthdaySuggestions",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_hide_birthday_suggestions",
	0 };
inline constexpr auto kDisableScrollToNextChannel = Option<bool>{
	"serein.disableScrollToNextChannel",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_disable_scroll_to_next_channel",
	0 };
inline constexpr auto kDisableScrollToNextTopic = Option<bool>{
	"serein.disableScrollToNextTopic",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_disable_scroll_to_next_topic",
	0 };
inline constexpr auto kRememberReadingPosition = Option<bool>{
	"serein.rememberReadingPosition",
	Scope::Device,
	false,
	Category::Chats,
	"lng_serein_remember_reading_position",
	0 };
inline const auto kRecentChats = Option<QString>{
	"serein.recentChats",
	Scope::Account,
	QString(),
	Category::Chats,
	"lng_serein_recent_chats",
	static_cast<unsigned>(Flag::Hidden),
	&ValidRecentChats };
inline const auto kReadingPositions = Option<QString>{
	"serein.readingPositions",
	Scope::Account,
	QString(),
	Category::Chats,
	"lng_serein_reading_positions",
	static_cast<unsigned>(Flag::Hidden),
	&ValidReadingPositions };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kCompactList));
	Expects(registry.Add(kPreviewLines));
	Expects(registry.Add(kHideSavedAndArchivedPreviews));
	Expects(registry.Add(kHideStories));
	Expects(registry.Add(kStartupFolderMode));
	Expects(registry.Add(kStartupFolderId));
	Expects(registry.Add(kLastOpenedFolderId));
	Expects(registry.Add(kHideAllChatsFolder));
	Expects(registry.Add(kShowArchiveInFolders));
	Expects(registry.Add(kHideFolderUnreadCounters));
	Expects(registry.Add(kChatSort));
	Expects(registry.Add(kManagedFolderIds));
	Expects(registry.Add(kHideSponsoredMessages));
	Expects(registry.Add(kHideProxySponsor));
	Expects(registry.Add(kHidePremiumPromotions));
	Expects(registry.Add(kHideBirthdaySuggestions));
	Expects(registry.Add(kDisableScrollToNextChannel));
	Expects(registry.Add(kDisableScrollToNextTopic));
	Expects(registry.Add(kRememberReadingPosition));
	Expects(registry.Add(kRecentChats));
	Expects(registry.Add(kReadingPositions));
}

} // namespace Serein::Chats
