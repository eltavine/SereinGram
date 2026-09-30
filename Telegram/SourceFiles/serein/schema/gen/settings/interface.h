// Generated from proto/serein/settings/v1/interface.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Interface {

[[nodiscard]] bool ValidMainMenuBytes(const QByteArray &value);

inline constexpr auto kBubbleRoundness = Option<int>{
	"serein.bubbleRoundness",
	Scope::Device,
	0,
	Category::Interface,
	"lng_serein_bubble_roundness",
	static_cast<unsigned>(Flag::RequiresRestart),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 10) && (value <= 100));
	} };
inline constexpr auto kAvatarRoundness = Option<int>{
	"serein.avatarRoundness",
	Scope::Device,
	0,
	Category::Interface,
	"lng_serein_avatar_roundness",
	static_cast<unsigned>(Flag::RequiresRestart),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 10) && (value <= 100));
	} };
inline constexpr auto kUniformAvatarShapes = Option<bool>{
	"serein.uniformAvatarShapes",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_uniform_avatar_shapes",
	static_cast<unsigned>(Flag::RequiresRestart) };
inline constexpr auto kTextMessageWidth = Option<int>{
	"serein.textMessageWidth",
	Scope::Device,
	0,
	Category::Interface,
	"lng_serein_text_message_width",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 50) && (value <= 400));
	} };
inline constexpr auto kWideChannelPosts = Option<bool>{
	"serein.wideChannelPosts",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_wide_channel_posts",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideBubbleTail = Option<bool>{
	"serein.hideBubbleTail",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_hide_bubble_tail",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kThemeReplyColors = Option<bool>{
	"serein.themeReplyColors",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_theme_reply_colors",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideReplyThumbnail = Option<bool>{
	"serein.hideReplyThumbnail",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_hide_reply_thumbnail",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kIgnoreChatTheme = Option<bool>{
	"serein.ignoreChatTheme",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_ignore_chat_theme",
	0 };
inline const auto kMainMenuConfig = Option<QByteArray>{
	"serein.mainMenu",
	Scope::Device,
	QByteArray(),
	Category::Interface,
	"lng_serein_main_menu",
	0,
	&ValidMainMenuBytes };
inline constexpr auto kHideAppIconBadge = Option<bool>{
	"serein.hideAppIconBadge",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_hide_app_icon_badge",
	0 };
inline constexpr auto kNotificationDelay = Option<int>{
	"serein.notificationDelay",
	Scope::Device,
	0,
	Category::Interface,
	"lng_serein_notification_delay",
	0,
	[](const int &value) {
		return (value == 0)
			|| (((value == 0 || value == 500 || value == 1000 || value == 2000 || value == 5000 || value == 10000 || value == 30000 || value == 60000)));
	} };
inline constexpr auto kOtherDeviceNotificationDelay = Option<int>{
	"serein.otherDeviceNotificationDelay",
	Scope::Device,
	0,
	Category::Interface,
	"lng_serein_other_device_notification_delay",
	0,
	[](const int &value) {
		return (value == 0)
			|| (((value == 0 || value == 500 || value == 1000 || value == 2000 || value == 5000 || value == 10000 || value == 30000 || value == 60000)));
	} };
inline constexpr auto kHalfwidthUiPunctuation = Option<bool>{
	"serein.halfwidthUiPunctuation",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_halfwidth_ui_punctuation",
	static_cast<unsigned>(Flag::RequiresRestart) };
inline constexpr auto kMoreAccounts = Option<bool>{
	"serein.moreAccounts",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_more_accounts",
	0 };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kBubbleRoundness));
	Expects(registry.Add(kAvatarRoundness));
	Expects(registry.Add(kUniformAvatarShapes));
	Expects(registry.Add(kTextMessageWidth));
	Expects(registry.Add(kWideChannelPosts));
	Expects(registry.Add(kHideBubbleTail));
	Expects(registry.Add(kThemeReplyColors));
	Expects(registry.Add(kHideReplyThumbnail));
	Expects(registry.Add(kIgnoreChatTheme));
	Expects(registry.Add(kMainMenuConfig));
	Expects(registry.Add(kHideAppIconBadge));
	Expects(registry.Add(kNotificationDelay));
	Expects(registry.Add(kOtherDeviceNotificationDelay));
	Expects(registry.Add(kHalfwidthUiPunctuation));
	Expects(registry.Add(kMoreAccounts));
}

} // namespace Serein::Interface
