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
		{  },
	},
	{
		&kHideEmojiButton,
		tr::lng_serein_hide_emoji_button,
		u"serein/compose/hide-emoji-button"_q,
		{  },
	},
	{
		&kHideRecordingButton,
		tr::lng_serein_hide_recording_button,
		u"serein/compose/hide-recording-button"_q,
		{  },
	},
	{
		&kHideBotCommandButton,
		tr::lng_serein_hide_bot_command_button,
		u"serein/compose/hide-bot-command-button"_q,
		{  },
	},
	{
		&kHideBotMenu,
		tr::lng_serein_hide_bot_menu,
		u"serein/compose/hide-bot-menu"_q,
		{  },
	},
	{
		&kHideAutoDeleteButton,
		tr::lng_serein_hide_auto_delete_button,
		u"serein/compose/hide-auto-delete-button"_q,
		{  },
	},
	{
		&kHideGiftButton,
		tr::lng_serein_hide_gift_button,
		u"serein/compose/hide-gift-button"_q,
		{  },
	},
	{
		&kHideAiButton,
		tr::lng_serein_hide_ai_button,
		u"serein/compose/hide-ai-button"_q,
		{  },
	},
	{
		&kHideSendAsButton,
		tr::lng_serein_hide_send_as_button,
		u"serein/compose/hide-send-as-button"_q,
		{  },
	},
	{
		&kHideStarsReactionButton,
		tr::lng_serein_hide_stars_reaction_button,
		u"serein/compose/hide-stars-reaction-button"_q,
		{  },
	},
	{
		&kHideChannelMuteButton,
		tr::lng_serein_hide_channel_mute_button,
		u"serein/compose/hide-channel-mute-button"_q,
		{  },
	},
	{
		&kDisableEmojiHover,
		tr::lng_serein_disable_emoji_hover,
		u"serein/compose/disable-emoji-hover"_q,
		{  },
	},
	{
		&kDisableAttachHover,
		tr::lng_serein_disable_attach_hover,
		u"serein/compose/disable-attach-hover"_q,
		{  },
	},
	{
		&kBotCommandsToDraft,
		tr::lng_serein_bot_commands_to_draft,
		u"serein/compose/bot-commands-to-draft"_q,
		{  },
	},
	{
		&kDisableAutoMarkdown,
		tr::lng_serein_disable_auto_markdown,
		u"serein/compose/disable-auto-markdown"_q,
		{  },
	},
	{
		&kDisableLinkPreview,
		tr::lng_serein_disable_link_preview,
		u"serein/compose/disable-link-preview"_q,
		{  },
	},
	{
		&kSpaceOnSend,
		tr::lng_serein_space_on_send,
		u"serein/compose/space-on-send"_q,
		{  },
	},
	{
		&kSpaceOnEdit,
		tr::lng_serein_space_on_edit,
		u"serein/compose/space-on-edit"_q,
		{  },
	},
	{
		&kConfirmSticker,
		tr::lng_serein_confirm_sticker,
		u"serein/compose/confirm-sticker"_q,
		{  },
	},
	{
		&kConfirmGif,
		tr::lng_serein_confirm_gif,
		u"serein/compose/confirm-gif"_q,
		{  },
	},
	{
		&kPreviewVoice,
		tr::lng_serein_preview_voice,
		u"serein/compose/preview-voice"_q,
		{  },
	},
	{
		&kPreviewRoundVideo,
		tr::lng_serein_preview_round_video,
		u"serein/compose/preview-round-video"_q,
		{  },
	},
	{
		&kConfirmPrivateCall,
		tr::lng_serein_confirm_private_call,
		u"serein/compose/confirm-private-call"_q,
		{  },
	},
	{
		&kForwardBeforeComment,
		tr::lng_serein_forward_before_comment,
		u"serein/compose/forward-before-comment"_q,
		{  },
	},
} };

} // namespace Serein::Compose
