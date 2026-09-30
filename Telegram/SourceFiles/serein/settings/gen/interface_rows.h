// Generated from proto/serein/settings/v1/interface.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Interface {

inline const auto kToggleRows = std::array<ToggleRow, 8>{ {
	{
		&kUniformAvatarShapes,
		tr::lng_serein_uniform_avatar_shapes,
		u"serein/interface/uniform-avatar-shapes"_q,
		{  },
	},
	{
		&kWideChannelPosts,
		tr::lng_serein_wide_channel_posts,
		u"serein/interface/wide-channel-posts"_q,
		{  },
	},
	{
		&kHideBubbleTail,
		tr::lng_serein_hide_bubble_tail,
		u"serein/interface/hide-bubble-tail"_q,
		{  },
	},
	{
		&kThemeReplyColors,
		tr::lng_serein_theme_reply_colors,
		u"serein/interface/theme-reply-colors"_q,
		{  },
	},
	{
		&kHideReplyThumbnail,
		tr::lng_serein_hide_reply_thumbnail,
		u"serein/interface/hide-reply-thumbnail"_q,
		{  },
	},
	{
		&kIgnoreChatTheme,
		tr::lng_serein_ignore_chat_theme,
		u"serein/interface/ignore-chat-theme"_q,
		{  },
	},
	{
		&kHideAppIconBadge,
		tr::lng_serein_hide_app_icon_badge,
		u"serein/interface/hide-app-icon-badge"_q,
		{  },
	},
	{
		&kHalfwidthUiPunctuation,
		tr::lng_serein_halfwidth_ui_punctuation,
		u"serein/interface/halfwidth-ui-punctuation"_q,
		{  },
	},
} };

struct CustomRows {
	CustomRow bubbleRoundness;
	CustomRow avatarRoundness;
	CustomRow textMessageWidth;
	CustomRow mainMenu;
	CustomRow notificationDelay;
	CustomRow otherDeviceNotificationDelay;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.bubbleRoundness();
	custom.avatarRoundness();
	AddToggle(builder, kToggleRows[0]);
	custom.textMessageWidth();
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	custom.mainMenu();
	AddToggle(builder, kToggleRows[6]);
	custom.notificationDelay();
	custom.otherDeviceNotificationDelay();
	AddToggle(builder, kToggleRows[7]);
}

} // namespace Serein::Interface
