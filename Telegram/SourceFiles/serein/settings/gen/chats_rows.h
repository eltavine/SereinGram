// Generated from proto/serein/settings/v1/chats.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Chats {

inline const auto kToggleRows = std::array<ToggleRow, 12>{ {
	{
		&kCompactList,
		tr::lng_serein_compact_chat_list,
		u"serein/chats/chat-list-compact"_q,
		{  },
	},
	{
		&kHideSavedAndArchivedPreviews,
		tr::lng_serein_hide_saved_and_archived_previews,
		u"serein/chats/hide-saved-and-archived-previews"_q,
		{  },
	},
	{
		&kHideStories,
		tr::lng_serein_hide_stories,
		u"serein/chats/hide-stories"_q,
		{  },
	},
	{
		&kHideAllChatsFolder,
		tr::lng_serein_hide_all_chats_folder,
		u"serein/chats/hide-all-chats-folder"_q,
		{  },
	},
	{
		&kShowArchiveInFolders,
		tr::lng_serein_show_archive_in_folders,
		u"serein/chats/show-archive-in-folders"_q,
		{  },
	},
	{
		&kHideFolderUnreadCounters,
		tr::lng_serein_hide_folder_unread_counters,
		u"serein/chats/hide-folder-unread-counters"_q,
		{  },
	},
	{
		&kHideSponsoredMessages,
		tr::lng_serein_hide_sponsored_messages,
		u"serein/chats/hide-sponsored-messages"_q,
		{  },
	},
	{
		&kHideProxySponsor,
		tr::lng_serein_hide_proxy_sponsor,
		u"serein/chats/hide-proxy-sponsor"_q,
		{  },
	},
	{
		&kHidePremiumPromotions,
		tr::lng_serein_hide_premium_promotions,
		u"serein/chats/hide-premium-promotions"_q,
		{  },
	},
	{
		&kHideBirthdaySuggestions,
		tr::lng_serein_hide_birthday_suggestions,
		u"serein/chats/hide-birthday-suggestions"_q,
		{  },
	},
	{
		&kDisableScrollToNextChannel,
		tr::lng_serein_disable_scroll_to_next_channel,
		u"serein/chats/disable-scroll-to-next-channel"_q,
		{  },
	},
	{
		&kDisableScrollToNextTopic,
		tr::lng_serein_disable_scroll_to_next_topic,
		u"serein/chats/disable-scroll-to-next-topic"_q,
		{  },
	},
} };

struct CustomRows {
	CustomRow chatPreviewLines;
	CustomRow startupFolderMode;
	CustomRow startupFolderId;
	CustomRow lastOpenedFolderId;
	CustomRow chatSort;
	CustomRow managedFolderIds;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddToggle(builder, kToggleRows[0]);
	custom.chatPreviewLines();
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	custom.startupFolderMode();
	custom.startupFolderId();
	custom.lastOpenedFolderId();
	custom.chatSort();
	custom.managedFolderIds();
	AddToggle(builder, kToggleRows[6]);
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
}

} // namespace Serein::Chats
