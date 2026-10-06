// Generated from proto/serein/settings/v1/interface.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Interface {

inline const auto kToggleRows = std::array<ToggleRow, 15>{ {
	{
		.option = &kWideChannelPosts,
		.title = tr::lng_serein_wide_channel_posts,
		.id = u"serein/interface/wide-channel-posts"_q,
		.keywords = { u"channel"_q, u"width"_q },
		.icon = &st::menuIconEnlarge,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_wide_channel_posts_about,
	},
	{
		.option = &kHideBubbleTail,
		.title = tr::lng_serein_hide_bubble_tail,
		.id = u"serein/interface/hide-bubble-tail"_q,
		.keywords = { u"bubble"_q, u"tail"_q },
		.icon = &st::menuIconChatBubble,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_bubble_tail_about,
	},
	{
		.option = &kThemeReplyColors,
		.title = tr::lng_serein_theme_reply_colors,
		.id = u"serein/interface/theme-reply-colors"_q,
		.keywords = { u"reply"_q, u"quote"_q, u"color"_q },
		.icon = &st::menuIconChangeColors,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_theme_reply_colors_about,
	},
	{
		.option = &kHideReplyThumbnail,
		.title = tr::lng_serein_hide_reply_thumbnail,
		.id = u"serein/interface/hide-reply-thumbnail"_q,
		.keywords = { u"reply"_q, u"thumbnail"_q },
		.icon = &st::menuIconReply,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_reply_thumbnail_about,
	},
	{
		.option = &kIgnoreChatTheme,
		.title = tr::lng_serein_ignore_chat_theme,
		.id = u"serein/interface/ignore-chat-theme"_q,
		.keywords = { u"chat"_q, u"theme"_q, u"wallpaper"_q },
		.icon = &st::menuIconPhoto,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_ignore_chat_theme_about,
	},
	{
		.option = &kMenuShortcuts,
		.title = tr::lng_serein_menu_shortcuts,
		.id = u"serein/interface/menu-shortcuts"_q,
		.keywords = { u"main menu"_q, u"tray"_q, u"ghost"_q, u"presentation"_q, u"recent chats"_q },
		.icon = &st::menuIconToMainMenu,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_menu_shortcuts_about,
	},
	{
		.option = &kCenterTopNotifications,
		.title = tr::lng_serein_center_top_notifications,
		.id = u"serein/interface/center-top-notifications"_q,
		.keywords = { u"notification"_q, u"position"_q, u"center"_q },
		.icon = &st::menuIconNotifications,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_center_top_notifications_about,
	},
	{
		.option = &kHideAppIconBadge,
		.title = tr::lng_serein_hide_app_icon_badge,
		.id = u"serein/interface/hide-app-icon-badge"_q,
		.keywords = { u"dock"_q, u"icon"_q, u"badge"_q },
		.icon = &st::menuIconDockBounce,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_app_icon_badge_about,
	},
	{
		.option = &kHalfwidthUiPunctuation,
		.title = tr::lng_serein_halfwidth_ui_punctuation,
		.id = u"serein/interface/halfwidth-ui-punctuation"_q,
		.keywords = { u"text"_q, u"punctuation"_q },
		.icon = &st::menuIconTranslate,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_halfwidth_ui_punctuation_about,
	},
	{
		.option = &kMoreAccounts,
		.title = tr::lng_serein_more_accounts,
		.id = u"serein/interface/more-accounts"_q,
		.keywords = { u"accounts"_q, u"limit"_q, u"multiple"_q },
		.icon = &st::menuIconAddAccount,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_more_accounts_about,
	},
	{
		.option = &kOnlineStatusInChats,
		.title = tr::lng_serein_online_status_in_chats,
		.id = u"serein/interface/online-status-in-chats"_q,
		.keywords = { u"online"_q, u"last seen"_q, u"chat list"_q },
		.icon = &st::menuIconChats,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_online_status_in_chats_about,
	},
	{
		.option = &kOnlineStatusInHeader,
		.title = tr::lng_serein_online_status_in_header,
		.id = u"serein/interface/online-status-in-header"_q,
		.keywords = { u"online"_q, u"last seen"_q, u"profile"_q, u"seconds"_q },
		.icon = &st::menuIconProfile,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_online_status_in_header_about,
	},
	{
		.option = &kOnlineStatusOnSenders,
		.title = tr::lng_serein_online_status_on_senders,
		.id = u"serein/interface/online-status-on-senders"_q,
		.keywords = { u"online"_q, u"avatar"_q, u"group"_q, u"sender"_q },
		.icon = &st::menuIconGroups,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_online_status_on_senders_about,
	},
	{
		.option = &kOnlineStatusSelf,
		.title = tr::lng_serein_online_status_self,
		.id = u"serein/interface/online-status-self"_q,
		.keywords = { u"online"_q, u"invisible"_q, u"ghost"_q, u"my status"_q },
		.icon = &st::menuIconStealth,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_online_status_self_about,
	},
	{
		.option = &kOnlineStatusDetailed,
		.title = tr::lng_serein_online_status_detailed,
		.id = u"serein/interface/online-status-detailed"_q,
		.keywords = { u"online"_q, u"last seen"_q, u"detailed"_q, u"exact time"_q },
		.icon = &st::menuIconWhenOnline,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_online_status_detailed_about,
	},
} };

struct CustomRows {
	CustomRow bubbleRoundness;
	CustomRow avatarRoundness;
	CustomRow uniformAvatarShapes;
	CustomRow mainMenu;
	CustomRow notificationDelay;
	CustomRow otherDeviceNotificationDelay;
	CustomRow quietHours;
	CustomRow globalShortcut;
	CustomRow checkUpdates;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddSection(builder, {
		u"serein/interface/roundness"_q,
		tr::lng_serein_roundness_and_shapes,
		{ u"corners"_q, u"shapes"_q },
	});
	custom.bubbleRoundness();
	custom.avatarRoundness();
	custom.uniformAvatarShapes();
	EndSection(builder);
	AddSection(builder, {
		u"serein/interface/message-style"_q,
		tr::lng_serein_message_style,
		{ u"messages"_q, u"style"_q },
	});
	AddNumber(builder, {
		.option = &kTextMessageWidth,
		.title = tr::lng_serein_text_message_width,
		.id = u"serein/interface/text-message-width"_q,
		.keywords = { u"text"_q, u"width"_q },
		.minimum = 50,
		.maximum = 400,
		.zeroLabel = tr::lng_serein_preview_follow,
		.format = [](int value) {
			return QString::number(value) + u"%"_q;
		},
		.hint = tr::lng_serein_text_width_hint,
		.icon = &st::menuIconFont,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_text_message_width_about,
	});
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/interface/main-menu-heading"_q,
		tr::lng_serein_main_menu,
		{ u"menu"_q, u"title"_q },
	});
	custom.mainMenu();
	AddToggle(builder, kToggleRows[5]);
	EndSection(builder, tr::lng_serein_menu_shortcuts_note);
	AddSection(builder, {
		u"serein/interface/notifications"_q,
		tr::lng_serein_section_notifications,
		{ u"notification"_q, u"window"_q },
	});
	custom.notificationDelay();
	custom.otherDeviceNotificationDelay();
	AddToggle(builder, kToggleRows[6]);
	AddNote(builder, tr::lng_serein_center_top_notifications_note);
	custom.quietHours();
	AddToggle(builder, kToggleRows[7]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/interface/app"_q,
		tr::lng_serein_section_app,
		{ u"shortcut"_q, u"accounts"_q, u"updates"_q },
	});
	custom.globalShortcut();
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddNote(builder, tr::lng_serein_more_accounts_note);
	custom.checkUpdates();
	EndSection(builder);
	AddSection(builder, {
		u"serein/interface/online"_q,
		tr::lng_serein_section_online_status,
		{ u"online"_q, u"last seen"_q, u"presence"_q },
	});
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	AddToggle(builder, kToggleRows[12]);
	AddToggle(builder, kToggleRows[13]);
	AddToggle(builder, kToggleRows[14]);
	EndSection(builder);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_interface;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_interface_about;
inline const auto kSubpageIcon = &st::menuIconPalette;
inline const auto kSubpageTile = &st::settingsIconBg6;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"interface"_q, u"appearance"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::Interface
