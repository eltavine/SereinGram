// Generated from proto/serein/settings/v1/chats.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Chats {

inline const auto kToggleRows = std::array<ToggleRow, 19>{ {
	{
		.option = &kCompactList,
		.title = tr::lng_serein_compact_chat_list,
		.id = u"serein/chats/chat-list-compact"_q,
		.keywords = { u"compact"_q, u"list"_q },
		.icon = &st::menuIconShrink,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_chat_list_compact_about,
	},
	{
		.option = &kHideSavedAndArchivedPreviews,
		.title = tr::lng_serein_hide_saved_and_archived_previews,
		.id = u"serein/chats/hide-saved-and-archived-previews"_q,
		.keywords = { u"saved"_q, u"archive"_q, u"preview"_q },
		.icon = &st::menuIconCaptionHide,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_saved_and_archived_previews_about,
	},
	{
		.option = &kHideStories,
		.title = tr::lng_serein_hide_stories,
		.id = u"serein/chats/hide-stories"_q,
		.keywords = { u"stories"_q },
		.icon = &st::menuIconStoriesSavedSection,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_stories_about,
	},
	{
		.option = &kSearchOwnChatsOnly,
		.title = tr::lng_serein_search_own_chats_only,
		.id = u"serein/chats/search-own-chats-only"_q,
		.keywords = { u"search"_q, u"global"_q, u"public"_q, u"privacy"_q },
		.icon = &st::menuIconSearch,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_search_own_chats_only_about,
	},
	{
		.option = &kHideAllChatsFolder,
		.title = tr::lng_serein_hide_all_chats_folder,
		.id = u"serein/chats/hide-all-chats-folder"_q,
		.keywords = { u"all chats"_q, u"folders"_q },
		.icon = &st::menuIconShowInFolder,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_all_chats_folder_about,
	},
	{
		.option = &kShowArchiveInFolders,
		.title = tr::lng_serein_show_archive_in_folders,
		.id = u"serein/chats/show-archive-in-folders"_q,
		.keywords = { u"archive"_q, u"folders"_q },
		.icon = &st::menuIconArchive,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_show_archive_in_folders_about,
	},
	{
		.option = &kHideFolderUnreadCounters,
		.title = tr::lng_serein_hide_folder_unread_counters,
		.id = u"serein/chats/hide-folder-unread-counters"_q,
		.keywords = { u"unread"_q, u"folders"_q },
		.icon = &st::menuIconMarkRead,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_folder_unread_counters_about,
	},
	{
		.option = &kManagedFolderFilter,
		.title = tr::lng_serein_managed_folder_filter,
		.id = u"serein/chats/managed-folder-filter"_q,
		.keywords = { u"folder"_q, u"admin"_q, u"manage"_q, u"groups"_q, u"channels"_q },
		.icon = &st::menuIconTagFilter,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_managed_folder_filter_about,
	},
	{
		.option = &kHideSponsoredMessages,
		.title = tr::lng_serein_hide_sponsored_messages,
		.id = u"serein/chats/hide-sponsored-messages"_q,
		.keywords = { u"sponsored"_q, u"search ads"_q },
		.icon = &st::menuIconBlock,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_sponsored_messages_about,
	},
	{
		.option = &kHideProxySponsor,
		.title = tr::lng_serein_hide_proxy_sponsor,
		.id = u"serein/chats/hide-proxy-sponsor"_q,
		.keywords = { u"proxy"_q, u"sponsored channel"_q },
		.icon = &st::menuIconNetwork,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_proxy_sponsor_about,
	},
	{
		.option = &kHidePremiumPromotions,
		.title = tr::lng_serein_hide_premium_promotions,
		.id = u"serein/chats/hide-premium-promotions"_q,
		.keywords = { u"Premium"_q, u"promotions"_q },
		.icon = &st::menuIconPremium,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_premium_promotions_about,
	},
	{
		.option = &kHideBirthdaySuggestions,
		.title = tr::lng_serein_hide_birthday_suggestions,
		.id = u"serein/chats/hide-birthday-suggestions"_q,
		.keywords = { u"birthday"_q, u"suggestion"_q },
		.icon = &st::menuIconGiftPremium,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_birthday_suggestions_about,
	},
	{
		.option = &kDisableScrollToNextChannel,
		.title = tr::lng_serein_disable_scroll_to_next_channel,
		.id = u"serein/chats/disable-scroll-to-next-channel"_q,
		.keywords = { u"scroll"_q, u"channel"_q },
		.icon = &st::menuIconChannel,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_disable_scroll_to_next_channel_about,
	},
	{
		.option = &kDisableScrollToNextTopic,
		.title = tr::lng_serein_disable_scroll_to_next_topic,
		.id = u"serein/chats/disable-scroll-to-next-topic"_q,
		.keywords = { u"scroll"_q, u"topic"_q },
		.icon = &st::menuIconTopics,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_disable_scroll_to_next_topic_about,
	},
	{
		.option = &kRememberReadingPosition,
		.title = tr::lng_serein_remember_reading_position,
		.id = u"serein/chats/remember-reading-position"_q,
		.keywords = { u"scroll"_q, u"position"_q, u"resume"_q },
		.icon = &st::menuIconSavedMessages,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_remember_reading_position_about,
	},
	{
		.option = &kChatQuickActions,
		.title = tr::lng_serein_chat_quick_actions,
		.id = u"serein/chats/chat-quick-actions"_q,
		.keywords = { u"toolbar"_q, u"search"_q, u"media"_q, u"pinned"_q },
		.icon = &st::menuIconBoosts,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_chat_quick_actions_about,
	},
	{
		.option = &kManagementShortcuts,
		.title = tr::lng_serein_management_shortcuts,
		.id = u"serein/chats/management-shortcuts"_q,
		.keywords = { u"admin"_q, u"members"_q, u"recent actions"_q, u"group"_q },
		.icon = &st::menuIconAdmin,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_management_shortcuts_about,
	},
	{
		.option = &kLocalPinning,
		.title = tr::lng_serein_local_pinning,
		.id = u"serein/chats/local-pinning"_q,
		.keywords = { u"pin"_q, u"top"_q, u"local"_q },
		.icon = &st::menuIconPin,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_local_pinning_about,
	},
	{
		.option = &kChatSettingsMenu,
		.title = tr::lng_serein_chat_settings_menu,
		.id = u"serein/chats/chat-settings-menu"_q,
		.keywords = { u"chat"_q, u"menu"_q, u"settings"_q },
		.icon = &st::menuIconSettings,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_chat_settings_menu_about,
	},
} };

struct CustomRows {
	CustomRow chatSort;
	CustomRow startupFolderMode;
	CustomRow hiddenFolderIds;
	CustomRow cleanup;
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
		.icon = &st::menuIconAsMessages,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_chat_preview_lines_about,
	});
	custom.chatSort();
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	EndSection(builder, tr::lng_serein_search_own_chats_only_note);
	AddSection(builder, {
		u"serein/chats/folders"_q,
		tr::lng_serein_folders,
		{ u"folders"_q },
	});
	custom.startupFolderMode();
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	custom.hiddenFolderIds();
	AddToggle(builder, kToggleRows[7]);
	EndSection(builder, tr::lng_serein_managed_folder_filter_note);
	AddSection(builder, {
		u"serein/chats/promotions"_q,
		tr::lng_serein_promotions,
		{ u"promotions"_q, u"ads"_q },
	});
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/chats/scroll-navigation"_q,
		tr::lng_serein_scroll_navigation,
		{ u"scroll"_q, u"navigation"_q },
	});
	AddToggle(builder, kToggleRows[12]);
	AddToggle(builder, kToggleRows[13]);
	AddToggle(builder, kToggleRows[14]);
	EndSection(builder, tr::lng_serein_remember_reading_position_note);
	AddSection(builder, {
		u"serein/chats/chat-tools"_q,
		tr::lng_serein_section_chat_tools,
		{ u"menu"_q, u"tools"_q, u"pin"_q },
	});
	AddToggle(builder, kToggleRows[15]);
	AddNote(builder, tr::lng_serein_chat_quick_actions_note);
	AddToggle(builder, kToggleRows[16]);
	AddNote(builder, tr::lng_serein_management_shortcuts_note);
	AddToggle(builder, kToggleRows[17]);
	AddNote(builder, tr::lng_serein_local_pinning_note);
	AddToggle(builder, kToggleRows[18]);
	AddNote(builder, tr::lng_serein_chat_settings_menu_note);
	custom.cleanup();
	EndSection(builder);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_chats;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_chats_about;
inline const auto kSubpageIcon = &st::menuIconChats;
inline const auto kSubpageTile = &st::settingsIconBg4;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"chats"_q, u"list"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::Chats
