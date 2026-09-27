#pragma once

#include "nagram/core/options.h"

namespace Nagram::Interface {

[[nodiscard]] bool ValidMainMenuBytes(const QByteArray &value);

inline constexpr auto kRestart = static_cast<unsigned>(Flag::RequiresRestart);
inline constexpr auto kBubbleRoundness = Option<int>{
	"nagram.bubbleRoundness", Scope::Device, 0,
	Category::Interface, "lng_nagram_bubble_roundness", kRestart,
	[](const int &value) { return !value || (value >= 10 && value <= 100); } };
inline constexpr auto kAvatarRoundness = Option<int>{
	"nagram.avatarRoundness", Scope::Device, 0,
	Category::Interface, "lng_nagram_avatar_roundness", kRestart,
	[](const int &value) { return !value || (value >= 10 && value <= 100); } };
inline constexpr auto kUniformAvatarShapes = Option<bool>{
	"nagram.uniformAvatarShapes", Scope::Device, false,
	Category::Interface, "lng_nagram_uniform_avatar_shapes", kRestart };
inline constexpr auto kTextMessageWidth = Option<int>{
	"nagram.textMessageWidth", Scope::Device, 0,
	Category::Interface, "lng_nagram_text_message_width",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const int &value) { return !value || (value >= 50 && value <= 400); } };
inline constexpr auto kWideChannelPosts = Option<bool>{
	"nagram.wideChannelPosts", Scope::Device, false,
	Category::Interface, "lng_nagram_wide_channel_posts",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideBubbleTail = Option<bool>{
	"nagram.hideBubbleTail", Scope::Device, false,
	Category::Interface, "lng_nagram_hide_bubble_tail",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kThemeReplyColors = Option<bool>{
	"nagram.themeReplyColors", Scope::Device, false,
	Category::Interface, "lng_nagram_theme_reply_colors",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideReplyThumbnail = Option<bool>{
	"nagram.hideReplyThumbnail", Scope::Device, false,
	Category::Interface, "lng_nagram_hide_reply_thumbnail",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kIgnoreChatTheme = Option<bool>{
	"nagram.ignoreChatTheme", Scope::Device, false,
	Category::Interface, "lng_nagram_ignore_chat_theme" };
inline const auto kMainMenuConfig = Option<QByteArray>{
	"nagram.mainMenu", Scope::Device, QByteArray(),
	Category::Interface, "lng_nagram_main_menu", 0,
	ValidMainMenuBytes };

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
}

} // namespace Nagram::Interface
