// Generated from proto/serein/settings/v1/chats.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QString>
#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::Chats {

[[nodiscard]] bool CompactList();
[[nodiscard]] rpl::producer<bool> CompactListValue();
[[nodiscard]] int PreviewLines();
[[nodiscard]] rpl::producer<int> PreviewLinesValue();
[[nodiscard]] bool HideSavedAndArchivedPreviews();
[[nodiscard]] rpl::producer<bool> HideSavedAndArchivedPreviewsValue();
[[nodiscard]] bool HideStories();
[[nodiscard]] rpl::producer<bool> HideStoriesValue();
[[nodiscard]] int StartupFolderMode(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> StartupFolderModeValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] int StartupFolderId(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> StartupFolderIdValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] int LastOpenedFolderId(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> LastOpenedFolderIdValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HideAllChatsFolder();
[[nodiscard]] rpl::producer<bool> HideAllChatsFolderValue();
[[nodiscard]] bool ShowArchiveInFolders();
[[nodiscard]] rpl::producer<bool> ShowArchiveInFoldersValue();
[[nodiscard]] bool HideFolderUnreadCounters();
[[nodiscard]] rpl::producer<bool> HideFolderUnreadCountersValue();
[[nodiscard]] int ChatSort();
[[nodiscard]] rpl::producer<int> ChatSortValue();
[[nodiscard]] bool ManagedFolderFilter();
[[nodiscard]] rpl::producer<bool> ManagedFolderFilterValue();
[[nodiscard]] QString ManagedFolderIds(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QString> ManagedFolderIdsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HideSponsoredMessages();
[[nodiscard]] rpl::producer<bool> HideSponsoredMessagesValue();
[[nodiscard]] bool HideProxySponsor();
[[nodiscard]] rpl::producer<bool> HideProxySponsorValue();
[[nodiscard]] bool HidePremiumPromotions();
[[nodiscard]] rpl::producer<bool> HidePremiumPromotionsValue();
[[nodiscard]] bool HideBirthdaySuggestions();
[[nodiscard]] rpl::producer<bool> HideBirthdaySuggestionsValue();
[[nodiscard]] bool DisableScrollToNextChannel();
[[nodiscard]] rpl::producer<bool> DisableScrollToNextChannelValue();
[[nodiscard]] bool DisableScrollToNextTopic();
[[nodiscard]] rpl::producer<bool> DisableScrollToNextTopicValue();
[[nodiscard]] bool RememberReadingPosition();
[[nodiscard]] rpl::producer<bool> RememberReadingPositionValue();
[[nodiscard]] bool ChatQuickActions();
[[nodiscard]] rpl::producer<bool> ChatQuickActionsValue();
[[nodiscard]] bool ManagementShortcuts();
[[nodiscard]] rpl::producer<bool> ManagementShortcutsValue();
[[nodiscard]] bool LocalPinning();
[[nodiscard]] rpl::producer<bool> LocalPinningValue();
[[nodiscard]] QString RecentChats(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QString> RecentChatsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] QString ReadingPositions(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QString> ReadingPositionsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] QString LocalPins(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QString> LocalPinsValue(gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks::Chats
