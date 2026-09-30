// Generated from proto/serein/settings/v1/compose.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/compose.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Compose {

inline const auto kToggleRows = std::array<ToggleRow, 24>{ {
	{
		&kHideAttachButton,
		tr::lng_serein_hide_attach_button,
		u"serein/compose/hide-attach-button"_q,
		{ u"attach"_q },
	},
	{
		&kHideEmojiButton,
		tr::lng_serein_hide_emoji_button,
		u"serein/compose/hide-emoji-button"_q,
		{ u"emoji"_q },
	},
	{
		&kHideRecordingButton,
		tr::lng_serein_hide_recording_button,
		u"serein/compose/hide-recording-button"_q,
		{ u"recording"_q },
	},
	{
		&kHideBotCommandButton,
		tr::lng_serein_hide_bot_command_button,
		u"serein/compose/hide-bot-command-button"_q,
		{ u"bot"_q, u"command"_q },
	},
	{
		&kHideBotMenu,
		tr::lng_serein_hide_bot_menu,
		u"serein/compose/hide-bot-menu"_q,
		{ u"bot"_q, u"menu"_q },
	},
	{
		&kHideAutoDeleteButton,
		tr::lng_serein_hide_auto_delete_button,
		u"serein/compose/hide-auto-delete-button"_q,
		{ u"delete"_q, u"timer"_q },
	},
	{
		&kHideGiftButton,
		tr::lng_serein_hide_gift_button,
		u"serein/compose/hide-gift-button"_q,
		{ u"gift"_q },
	},
	{
		&kHideAiButton,
		tr::lng_serein_hide_ai_button,
		u"serein/compose/hide-ai-button"_q,
		{ u"AI"_q },
	},
	{
		&kHideSendAsButton,
		tr::lng_serein_hide_send_as_button,
		u"serein/compose/hide-send-as-button"_q,
		{ u"send as"_q },
	},
	{
		&kHideStarsReactionButton,
		tr::lng_serein_hide_stars_reaction_button,
		u"serein/compose/hide-stars-reaction-button"_q,
		{ u"Stars"_q },
	},
	{
		&kHideChannelMuteButton,
		tr::lng_serein_hide_channel_mute_button,
		u"serein/compose/hide-channel-mute-button"_q,
		{ u"channel"_q, u"mute"_q },
	},
	{
		&kDisableEmojiHover,
		tr::lng_serein_disable_emoji_hover,
		u"serein/compose/disable-emoji-hover"_q,
		{ u"emoji"_q, u"hover"_q },
	},
	{
		&kDisableAttachHover,
		tr::lng_serein_disable_attach_hover,
		u"serein/compose/disable-attach-hover"_q,
		{ u"attachment"_q, u"hover"_q },
	},
	{
		&kBotCommandsToDraft,
		tr::lng_serein_bot_commands_to_draft,
		u"serein/compose/bot-commands-to-draft"_q,
		{ u"bot"_q, u"command"_q, u"draft"_q },
	},
	{
		&kDisableAutoMarkdown,
		tr::lng_serein_disable_auto_markdown,
		u"serein/compose/disable-auto-markdown"_q,
		{ u"Markdown"_q },
	},
	{
		&kDisableLinkPreview,
		tr::lng_serein_disable_link_preview,
		u"serein/compose/disable-link-preview"_q,
		{ u"link"_q, u"preview"_q },
	},
	{
		&kSpaceOnSend,
		tr::lng_serein_space_on_send,
		u"serein/compose/space-on-send"_q,
		{ u"spacing"_q, u"send"_q },
	},
	{
		&kSpaceOnEdit,
		tr::lng_serein_space_on_edit,
		u"serein/compose/space-on-edit"_q,
		{ u"spacing"_q, u"edit"_q },
	},
	{
		&kConfirmSticker,
		tr::lng_serein_confirm_sticker,
		u"serein/compose/confirm-sticker"_q,
		{ u"sticker"_q, u"confirm"_q },
	},
	{
		&kConfirmGif,
		tr::lng_serein_confirm_gif,
		u"serein/compose/confirm-gif"_q,
		{ u"GIF"_q, u"confirm"_q },
	},
	{
		&kPreviewVoice,
		tr::lng_serein_preview_voice,
		u"serein/compose/preview-voice"_q,
		{ u"voice"_q, u"listen"_q },
	},
	{
		&kPreviewRoundVideo,
		tr::lng_serein_preview_round_video,
		u"serein/compose/preview-round-video"_q,
		{ u"video"_q, u"preview"_q },
	},
	{
		&kConfirmPrivateCall,
		tr::lng_serein_confirm_private_call,
		u"serein/compose/confirm-private-call"_q,
		{ u"call"_q, u"confirm"_q },
	},
	{
		&kForwardBeforeComment,
		tr::lng_serein_forward_before_comment,
		u"serein/compose/forward-before-comment"_q,
		{ u"forward"_q, u"comment"_q, u"order"_q },
	},
} };

struct CustomRows {
	CustomRow inputPlaceholderMode;
	CustomRow defaultCodeLanguage;
	CustomRow quickReplies;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddSection(builder, {
		u"serein/compose/buttons"_q,
		tr::lng_serein_compose_buttons,
		{ u"buttons"_q, u"compose"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddSection(builder, {
		u"serein/compose/input-behavior"_q,
		tr::lng_serein_input_behavior,
		{ u"input"_q, u"behavior"_q },
	});
	AddToggle(builder, kToggleRows[11]);
	AddToggle(builder, kToggleRows[12]);
	AddToggle(builder, kToggleRows[13]);
	custom.inputPlaceholderMode();
	AddSection(builder, {
		u"serein/compose/text-format"_q,
		tr::lng_serein_text_format,
		{ u"text"_q, u"format"_q },
	});
	AddToggle(builder, kToggleRows[14]);
	AddToggle(builder, kToggleRows[15]);
	AddToggle(builder, kToggleRows[16]);
	AddToggle(builder, kToggleRows[17]);
	custom.defaultCodeLanguage();
	custom.quickReplies();
	AddSection(builder, {
		u"serein/compose/send-confirmation"_q,
		tr::lng_serein_send_confirmation,
		{ u"send"_q, u"confirm"_q },
	});
	AddToggle(builder, kToggleRows[18]);
	AddToggle(builder, kToggleRows[19]);
	AddToggle(builder, kToggleRows[20]);
	AddToggle(builder, kToggleRows[21]);
	AddToggle(builder, kToggleRows[22]);
	AddSection(builder, {
		u"serein/compose/forwarding"_q,
		tr::lng_serein_forwarding,
		{ u"forward"_q },
	});
	AddToggle(builder, kToggleRows[23]);
}

} // namespace Serein::Compose
