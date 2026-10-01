// Generated from proto/serein/settings/v1/chats.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Chats {

inline const auto kToggleRows = std::array<ToggleRow, 17>{ {
	{
		&kCompactList,
		tr::lng_serein_compact_chat_list,
		u"serein/chats/chat-list-compact"_q,
		{ u"compact"_q, u"list"_q },
	},
	{
		&kHideSavedAndArchivedPreviews,
		tr::lng_serein_hide_saved_and_archived_previews,
		u"serein/chats/hide-saved-and-archived-previews"_q,
		{ u"saved"_q, u"archive"_q, u"preview"_q },
	},
	{
		&kHideStories,
		tr::lng_serein_hide_stories,
		u"serein/chats/hide-stories"_q,
		{ u"stories"_q },
	},
	{
		&kHideAllChatsFolder,
		tr::lng_serein_hide_all_chats_folder,
		u"serein/chats/hide-all-chats-folder"_q,
		{ u"all chats"_q, u"folders"_q },
	},
	{
		&kShowArchiveInFolders,
		tr::lng_serein_show_archive_in_folders,
		u"serein/chats/show-archive-in-folders"_q,
		{ u"archive"_q, u"folders"_q },
	},
	{
		&kHideFolderUnreadCounters,
		tr::lng_serein_hide_folder_unread_counters,
		u"serein/chats/hide-folder-unread-counters"_q,
		{ u"unread"_q, u"folders"_q },
	},
	{
		&kManagedFolderFilter,
		tr::lng_serein_managed_folder_filter,
		u"serein/chats/managed-folder-filter"_q,
		{ u"folder"_q, u"admin"_q, u"manage"_q, u"groups"_q, u"channels"_q },
	},
	{
		&kHideSponsoredMessages,
		tr::lng_serein_hide_sponsored_messages,
		u"serein/chats/hide-sponsored-messages"_q,
		{ u"sponsored"_q, u"search ads"_q },
	},
	{
		&kHideProxySponsor,
		tr::lng_serein_hide_proxy_sponsor,
		u"serein/chats/hide-proxy-sponsor"_q,
		{ u"proxy"_q, u"sponsored channel"_q },
	},
	{
		&kHidePremiumPromotions,
		tr::lng_serein_hide_premium_promotions,
		u"serein/chats/hide-premium-promotions"_q,
		{ u"Premium"_q, u"promotions"_q },
	},
	{
		&kHideBirthdaySuggestions,
		tr::lng_serein_hide_birthday_suggestions,
		u"serein/chats/hide-birthday-suggestions"_q,
		{ u"birthday"_q, u"suggestion"_q },
	},
	{
		&kDisableScrollToNextChannel,
		tr::lng_serein_disable_scroll_to_next_channel,
		u"serein/chats/disable-scroll-to-next-channel"_q,
		{ u"scroll"_q, u"channel"_q },
	},
	{
		&kDisableScrollToNextTopic,
		tr::lng_serein_disable_scroll_to_next_topic,
		u"serein/chats/disable-scroll-to-next-topic"_q,
		{ u"scroll"_q, u"topic"_q },
	},
	{
		&kRememberReadingPosition,
		tr::lng_serein_remember_reading_position,
		u"serein/chats/remember-reading-position"_q,
		{ u"scroll"_q, u"position"_q, u"resume"_q },
	},
	{
		&kChatQuickActions,
		tr::lng_serein_chat_quick_actions,
		u"serein/chats/chat-quick-actions"_q,
		{ u"toolbar"_q, u"search"_q, u"media"_q, u"pinned"_q },
	},
	{
		&kManagementShortcuts,
		tr::lng_serein_management_shortcuts,
		u"serein/chats/management-shortcuts"_q,
		{ u"admin"_q, u"members"_q, u"recent actions"_q, u"group"_q },
	},
	{
		&kLocalPinning,
		tr::lng_serein_local_pinning,
		u"serein/chats/local-pinning"_q,
		{ u"pin"_q, u"top"_q, u"local"_q },
	},
} };

struct CustomRows {
	CustomRow startupFolderMode;
	CustomRow chatSort;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddSection(builder, {
		u"serein/chats/list"_q,
		tr::lng_serein_chat_list,
		{ u"list"_q, u"layout"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddChoice(builder, {
		.option = &kPreviewLines,
		.title = tr::lng_serein_chat_preview_lines,
		.id = u"serein/chats/chat-preview-lines"_q,
		.keywords = { u"preview"_q, u"lines"_q },
		.values = { 0, 1, 2, 3 },
		.labels = { tr::lng_serein_preview_follow, tr::lng_serein_preview_one, tr::lng_serein_preview_two, tr::lng_serein_preview_three },
	});
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddSection(builder, {
		u"serein/chats/folders"_q,
		tr::lng_serein_folders,
		{ u"folders"_q },
	});
	custom.startupFolderMode();
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddSection(builder, {
		u"serein/chats/sorting"_q,
		tr::lng_serein_sorting,
		{ u"sort"_q, u"order"_q },
	});
	custom.chatSort();
	AddToggle(builder, kToggleRows[6]);
	AddNote(builder, tr::lng_serein_managed_folder_filter_note);
	AddSection(builder, {
		u"serein/chats/promotions"_q,
		tr::lng_serein_promotions,
		{ u"promotions"_q, u"ads"_q },
	});
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddSection(builder, {
		u"serein/chats/scroll-navigation"_q,
		tr::lng_serein_scroll_navigation,
		{ u"scroll"_q, u"navigation"_q },
	});
	AddToggle(builder, kToggleRows[11]);
	AddToggle(builder, kToggleRows[12]);
	AddToggle(builder, kToggleRows[13]);
	AddNote(builder, tr::lng_serein_remember_reading_position_note);
	AddToggle(builder, kToggleRows[14]);
	AddNote(builder, tr::lng_serein_chat_quick_actions_note);
	AddToggle(builder, kToggleRows[15]);
	AddNote(builder, tr::lng_serein_management_shortcuts_note);
	AddToggle(builder, kToggleRows[16]);
	AddNote(builder, tr::lng_serein_local_pinning_note);
}

} // namespace Serein::Chats
