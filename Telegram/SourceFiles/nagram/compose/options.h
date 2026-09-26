#pragma once

#include "nagram/core/options.h"

namespace Nagram::Compose {

inline constexpr auto kRefreshButtons = static_cast<unsigned>(
	Flag::RefreshComposeButtons);

inline constexpr auto kHideAttachButton = Option<bool>{
	"nagram.hideAttachButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_attach_button", kRefreshButtons };
inline constexpr auto kHideEmojiButton = Option<bool>{
	"nagram.hideEmojiButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_emoji_button", kRefreshButtons };
inline constexpr auto kHideRecordingButton = Option<bool>{
	"nagram.hideRecordingButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_recording_button", kRefreshButtons };
inline constexpr auto kHideBotCommandButton = Option<bool>{
	"nagram.hideBotCommandButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_bot_command_button", kRefreshButtons };
inline constexpr auto kHideBotMenu = Option<bool>{
	"nagram.hideBotMenu", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_bot_menu", kRefreshButtons };
inline constexpr auto kHideAutoDeleteButton = Option<bool>{
	"nagram.hideAutoDeleteButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_auto_delete_button", kRefreshButtons };
inline constexpr auto kHideGiftButton = Option<bool>{
	"nagram.hideGiftButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_gift_button", kRefreshButtons };
inline constexpr auto kHideAiButton = Option<bool>{
	"nagram.hideAiButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_ai_button", kRefreshButtons };
inline constexpr auto kHideSendAsButton = Option<bool>{
	"nagram.hideSendAsButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_send_as_button", kRefreshButtons };
inline constexpr auto kHideStarsReactionButton = Option<bool>{
	"nagram.hideStarsReactionButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_stars_reaction_button", kRefreshButtons };
inline constexpr auto kHideChannelMuteButton = Option<bool>{
	"nagram.hideChannelMuteButton", Scope::Device, false,
	Category::Compose, "lng_nagram_hide_channel_mute_button", kRefreshButtons };
inline constexpr auto kDisableEmojiHover = Option<bool>{
	"nagram.disableEmojiHover", Scope::Device, false,
	Category::Compose, "lng_nagram_disable_emoji_hover" };
inline constexpr auto kDisableAttachHover = Option<bool>{
	"nagram.disableAttachHover", Scope::Device, false,
	Category::Compose, "lng_nagram_disable_attach_hover" };
inline constexpr auto kBotCommandsToDraft = Option<bool>{
	"nagram.botCommandsToDraft", Scope::Device, false,
	Category::Compose, "lng_nagram_bot_commands_to_draft" };
inline constexpr auto kInputPlaceholderMode = Option<int>{
	"nagram.inputPlaceholderMode", Scope::Device, 0,
	Category::Compose, "lng_nagram_input_placeholder", 0,
	[](const int &value) { return value >= 0 && value <= 2; } };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kHideAttachButton));
	Expects(registry.Add(kHideEmojiButton));
	Expects(registry.Add(kHideRecordingButton));
	Expects(registry.Add(kHideBotCommandButton));
	Expects(registry.Add(kHideBotMenu));
	Expects(registry.Add(kHideAutoDeleteButton));
	Expects(registry.Add(kHideGiftButton));
	Expects(registry.Add(kHideAiButton));
	Expects(registry.Add(kHideSendAsButton));
	Expects(registry.Add(kHideStarsReactionButton));
	Expects(registry.Add(kHideChannelMuteButton));
	Expects(registry.Add(kDisableEmojiHover));
	Expects(registry.Add(kDisableAttachHover));
	Expects(registry.Add(kBotCommandsToDraft));
	Expects(registry.Add(kInputPlaceholderMode));
}

} // namespace Nagram::Compose
