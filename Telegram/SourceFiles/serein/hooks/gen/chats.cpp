// Generated from proto/serein/settings/v1/chats.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/chats.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/chats.h"

namespace Serein::Hooks::Chats {

bool CompactList() {
	return ForDevice().Get(Serein::Chats::kCompactList);
}

rpl::producer<bool> CompactListValue() {
	return ForDevice().Value(Serein::Chats::kCompactList);
}

int PreviewLines() {
	return ForDevice().Get(Serein::Chats::kPreviewLines);
}

rpl::producer<int> PreviewLinesValue() {
	return ForDevice().Value(Serein::Chats::kPreviewLines);
}

bool HideSavedAndArchivedPreviews() {
	return ForDevice().Get(Serein::Chats::kHideSavedAndArchivedPreviews);
}

rpl::producer<bool> HideSavedAndArchivedPreviewsValue() {
	return ForDevice().Value(Serein::Chats::kHideSavedAndArchivedPreviews);
}

bool HideStories() {
	return ForDevice().Get(Serein::Chats::kHideStories);
}

rpl::producer<bool> HideStoriesValue() {
	return ForDevice().Value(Serein::Chats::kHideStories);
}

int StartupFolderMode(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Chats::kStartupFolderMode);
}

rpl::producer<int> StartupFolderModeValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Chats::kStartupFolderMode);
}

int StartupFolderId(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Chats::kStartupFolderId);
}

rpl::producer<int> StartupFolderIdValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Chats::kStartupFolderId);
}

int LastOpenedFolderId(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Chats::kLastOpenedFolderId);
}

rpl::producer<int> LastOpenedFolderIdValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Chats::kLastOpenedFolderId);
}

bool HideAllChatsFolder() {
	return ForDevice().Get(Serein::Chats::kHideAllChatsFolder);
}

rpl::producer<bool> HideAllChatsFolderValue() {
	return ForDevice().Value(Serein::Chats::kHideAllChatsFolder);
}

bool ShowArchiveInFolders() {
	return ForDevice().Get(Serein::Chats::kShowArchiveInFolders);
}

rpl::producer<bool> ShowArchiveInFoldersValue() {
	return ForDevice().Value(Serein::Chats::kShowArchiveInFolders);
}

bool HideFolderUnreadCounters() {
	return ForDevice().Get(Serein::Chats::kHideFolderUnreadCounters);
}

rpl::producer<bool> HideFolderUnreadCountersValue() {
	return ForDevice().Value(Serein::Chats::kHideFolderUnreadCounters);
}

int ChatSort() {
	return ForDevice().Get(Serein::Chats::kChatSort);
}

rpl::producer<int> ChatSortValue() {
	return ForDevice().Value(Serein::Chats::kChatSort);
}

QString ManagedFolderIds(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Chats::kManagedFolderIds);
}

rpl::producer<QString> ManagedFolderIdsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Chats::kManagedFolderIds);
}

bool HideSponsoredMessages() {
	return ForDevice().Get(Serein::Chats::kHideSponsoredMessages);
}

rpl::producer<bool> HideSponsoredMessagesValue() {
	return ForDevice().Value(Serein::Chats::kHideSponsoredMessages);
}

bool HideProxySponsor() {
	return ForDevice().Get(Serein::Chats::kHideProxySponsor);
}

rpl::producer<bool> HideProxySponsorValue() {
	return ForDevice().Value(Serein::Chats::kHideProxySponsor);
}

bool HidePremiumPromotions() {
	return ForDevice().Get(Serein::Chats::kHidePremiumPromotions);
}

rpl::producer<bool> HidePremiumPromotionsValue() {
	return ForDevice().Value(Serein::Chats::kHidePremiumPromotions);
}

bool HideBirthdaySuggestions() {
	return ForDevice().Get(Serein::Chats::kHideBirthdaySuggestions);
}

rpl::producer<bool> HideBirthdaySuggestionsValue() {
	return ForDevice().Value(Serein::Chats::kHideBirthdaySuggestions);
}

bool DisableScrollToNextChannel() {
	return ForDevice().Get(Serein::Chats::kDisableScrollToNextChannel);
}

rpl::producer<bool> DisableScrollToNextChannelValue() {
	return ForDevice().Value(Serein::Chats::kDisableScrollToNextChannel);
}

bool DisableScrollToNextTopic() {
	return ForDevice().Get(Serein::Chats::kDisableScrollToNextTopic);
}

rpl::producer<bool> DisableScrollToNextTopicValue() {
	return ForDevice().Value(Serein::Chats::kDisableScrollToNextTopic);
}

} // namespace Serein::Hooks::Chats
