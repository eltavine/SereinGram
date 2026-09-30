// Generated from proto/serein/settings/v1/interface.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Interface {

inline const auto kToggleRows = std::array<ToggleRow, 9>{ {
	{
		&kWideChannelPosts,
		tr::lng_serein_wide_channel_posts,
		u"serein/interface/wide-channel-posts"_q,
		{ u"channel"_q, u"width"_q },
	},
	{
		&kHideBubbleTail,
		tr::lng_serein_hide_bubble_tail,
		u"serein/interface/hide-bubble-tail"_q,
		{ u"bubble"_q, u"tail"_q },
	},
	{
		&kThemeReplyColors,
		tr::lng_serein_theme_reply_colors,
		u"serein/interface/theme-reply-colors"_q,
		{ u"reply"_q, u"quote"_q, u"color"_q },
	},
	{
		&kHideReplyThumbnail,
		tr::lng_serein_hide_reply_thumbnail,
		u"serein/interface/hide-reply-thumbnail"_q,
		{ u"reply"_q, u"thumbnail"_q },
	},
	{
		&kIgnoreChatTheme,
		tr::lng_serein_ignore_chat_theme,
		u"serein/interface/ignore-chat-theme"_q,
		{ u"chat"_q, u"theme"_q, u"wallpaper"_q },
	},
	{
		&kHideAppIconBadge,
		tr::lng_serein_hide_app_icon_badge,
		u"serein/interface/hide-app-icon-badge"_q,
		{ u"dock"_q, u"icon"_q, u"badge"_q },
	},
	{
		&kHalfwidthUiPunctuation,
		tr::lng_serein_halfwidth_ui_punctuation,
		u"serein/interface/halfwidth-ui-punctuation"_q,
		{ u"text"_q, u"punctuation"_q },
	},
	{
		&kMoreAccounts,
		tr::lng_serein_more_accounts,
		u"serein/interface/more-accounts"_q,
		{ u"accounts"_q, u"limit"_q, u"multiple"_q },
	},
	{
		&kCheckUpdates,
		tr::lng_serein_check_updates,
		u"serein/interface/check-updates"_q,
		{ u"updates"_q, u"GitHub"_q, u"version"_q },
	},
} };

struct CustomRows {
	CustomRow bubbleRoundness;
	CustomRow avatarRoundness;
	CustomRow uniformAvatarShapes;
	CustomRow mainMenu;
	CustomRow notificationDelay;
	CustomRow otherDeviceNotificationDelay;
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
	});
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddSection(builder, {
		u"serein/interface/main-menu-heading"_q,
		tr::lng_serein_main_menu,
		{ u"menu"_q, u"title"_q },
	});
	custom.mainMenu();
	AddSection(builder, {
		u"serein/interface/window-notification"_q,
		tr::lng_serein_window_notification,
		{ u"window"_q, u"notification"_q },
	});
	AddToggle(builder, kToggleRows[5]);
	custom.notificationDelay();
	custom.otherDeviceNotificationDelay();
	AddSection(builder, {
		u"serein/interface/text"_q,
		tr::lng_serein_ui_text,
		{ u"text"_q, u"punctuation"_q },
	});
	AddToggle(builder, kToggleRows[6]);
	AddSection(builder, {
		u"serein/interface/accounts"_q,
		tr::lng_serein_accounts,
		{ u"accounts"_q },
	});
	AddToggle(builder, kToggleRows[7]);
	AddNote(builder, tr::lng_serein_more_accounts_note);
	AddSection(builder, {
		u"serein/interface/updates"_q,
		tr::lng_serein_updates,
		{ u"updates"_q },
	});
	AddToggle(builder, kToggleRows[8]);
}

} // namespace Serein::Interface
