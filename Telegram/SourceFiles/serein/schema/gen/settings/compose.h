// Generated from proto/serein/settings/v1/compose.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"
#include "serein/schema/codec.h"

namespace Serein::Compose {

[[nodiscard]] bool ValidQuickReplies(const QByteArray &value);
[[nodiscard]] bool ValidTextReplacementsBytes(const QByteArray &value);

inline constexpr auto kHideAttachButton = Option<bool>{
	"serein.hideAttachButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_attach_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideEmojiButton = Option<bool>{
	"serein.hideEmojiButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_emoji_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideRecordingButton = Option<bool>{
	"serein.hideRecordingButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_recording_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideBotCommandButton = Option<bool>{
	"serein.hideBotCommandButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_bot_command_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideBotMenu = Option<bool>{
	"serein.hideBotMenu",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_bot_menu",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideAutoDeleteButton = Option<bool>{
	"serein.hideAutoDeleteButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_auto_delete_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideGiftButton = Option<bool>{
	"serein.hideGiftButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_gift_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideAiButton = Option<bool>{
	"serein.hideAiButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_ai_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideSendAsButton = Option<bool>{
	"serein.hideSendAsButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_send_as_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideStarsReactionButton = Option<bool>{
	"serein.hideStarsReactionButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_stars_reaction_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kHideChannelMuteButton = Option<bool>{
	"serein.hideChannelMuteButton",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_hide_channel_mute_button",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline constexpr auto kDisableEmojiHover = Option<bool>{
	"serein.disableEmojiHover",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_disable_emoji_hover",
	0 };
inline constexpr auto kDisableAttachHover = Option<bool>{
	"serein.disableAttachHover",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_disable_attach_hover",
	0 };
inline constexpr auto kBotCommandsToDraft = Option<bool>{
	"serein.botCommandsToDraft",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_bot_commands_to_draft",
	0 };
inline constexpr auto kInputPlaceholderMode = Option<int>{
	"serein.inputPlaceholderMode",
	Scope::Device,
	0,
	Category::Compose,
	"lng_serein_input_placeholder",
	0,
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 2));
	} };
inline constexpr auto kDisableAutoMarkdown = Option<bool>{
	"serein.disableAutoMarkdown",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_disable_auto_markdown",
	0 };
inline constexpr auto kDisableLinkPreview = Option<bool>{
	"serein.disableLinkPreview",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_disable_link_preview",
	0 };
inline constexpr auto kSpaceOnSend = Option<bool>{
	"serein.spaceOnSend",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_space_on_send",
	0 };
inline constexpr auto kSpaceOnEdit = Option<bool>{
	"serein.spaceOnEdit",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_space_on_edit",
	0 };
inline constexpr auto kMentionMenu = Option<bool>{
	"serein.mentionMenu",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_mention_menu",
	0 };
inline constexpr auto kFormatToolbar = Option<bool>{
	"serein.formatToolbar",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_format_toolbar",
	0 };
inline const auto kDefaultCodeLanguage = Option<QString>{
	"serein.defaultCodeLanguage",
	Scope::Device,
	QString(),
	Category::Compose,
	"lng_serein_default_code_language",
	0,
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 32) && (Codec::Matches(value, QString::fromUtf8("^[A-Za-z0-9+-]*$"))));
	} };
inline const auto kQuickReplies = Option<QByteArray>{
	"serein.quickReplies",
	Scope::Device,
	QByteArray("{\"version\":1,\"replies\":[\"\",\"\"]}"),
	Category::Compose,
	"lng_serein_quick_replies",
	0,
	&ValidQuickReplies };
inline const auto kTextReplacements = Option<QByteArray>{
	"serein.textReplacements",
	Scope::Device,
	QByteArray(),
	Category::Compose,
	"lng_serein_text_replacements",
	0,
	&ValidTextReplacementsBytes };
inline constexpr auto kConfirmSticker = Option<bool>{
	"serein.confirmSticker",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_confirm_sticker",
	0 };
inline constexpr auto kConfirmGif = Option<bool>{
	"serein.confirmGif",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_confirm_gif",
	0 };
inline constexpr auto kPreviewVoice = Option<bool>{
	"serein.previewVoice",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_preview_voice",
	0 };
inline constexpr auto kPreviewRoundVideo = Option<bool>{
	"serein.previewRoundVideo",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_preview_round_video",
	0 };
inline constexpr auto kConfirmPrivateCall = Option<bool>{
	"serein.confirmPrivateCall",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_confirm_private_call",
	0 };
inline constexpr auto kForwardBeforeComment = Option<bool>{
	"serein.forwardBeforeComment",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_forward_before_comment",
	0 };
inline constexpr auto kSendSilently = Option<bool>{
	"serein.sendSilently",
	Scope::Device,
	false,
	Category::Compose,
	"lng_serein_send_silently",
	0 };

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
	Expects(registry.Add(kDisableAutoMarkdown));
	Expects(registry.Add(kDisableLinkPreview));
	Expects(registry.Add(kSpaceOnSend));
	Expects(registry.Add(kSpaceOnEdit));
	Expects(registry.Add(kMentionMenu));
	Expects(registry.Add(kFormatToolbar));
	Expects(registry.Add(kDefaultCodeLanguage));
	Expects(registry.Add(kQuickReplies));
	Expects(registry.Add(kTextReplacements));
	Expects(registry.Add(kConfirmSticker));
	Expects(registry.Add(kConfirmGif));
	Expects(registry.Add(kPreviewVoice));
	Expects(registry.Add(kPreviewRoundVideo));
	Expects(registry.Add(kConfirmPrivateCall));
	Expects(registry.Add(kForwardBeforeComment));
	Expects(registry.Add(kSendSilently));
}

} // namespace Serein::Compose
