// Generated from proto/serein/settings/v1/compose.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/compose.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Compose {

inline const auto kToggleRows = std::array<ToggleRow, 31>{ {
	{
		.option = &kHideAttachButton,
		.title = tr::lng_serein_hide_attach_button,
		.id = u"serein/compose/hide-attach-button"_q,
		.keywords = { u"attach"_q },
		.icon = &st::menuIconFile,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_attach_button_about,
	},
	{
		.option = &kHideEmojiButton,
		.title = tr::lng_serein_hide_emoji_button,
		.id = u"serein/compose/hide-emoji-button"_q,
		.keywords = { u"emoji"_q },
		.icon = &st::menuIconEmoji,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_emoji_button_about,
	},
	{
		.option = &kHideRecordingButton,
		.title = tr::lng_serein_hide_recording_button,
		.id = u"serein/compose/hide-recording-button"_q,
		.keywords = { u"recording"_q },
		.icon = &st::menuIconStartStreamWith,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_recording_button_about,
	},
	{
		.option = &kHideBotCommandButton,
		.title = tr::lng_serein_hide_bot_command_button,
		.id = u"serein/compose/hide-bot-command-button"_q,
		.keywords = { u"bot"_q, u"command"_q },
		.icon = &st::menuIconBotCommands,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_bot_command_button_about,
	},
	{
		.option = &kHideBotMenu,
		.title = tr::lng_serein_hide_bot_menu,
		.id = u"serein/compose/hide-bot-menu"_q,
		.keywords = { u"bot"_q, u"menu"_q },
		.icon = &st::menuIconBot,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_bot_menu_about,
	},
	{
		.option = &kHideAutoDeleteButton,
		.title = tr::lng_serein_hide_auto_delete_button,
		.id = u"serein/compose/hide-auto-delete-button"_q,
		.keywords = { u"delete"_q, u"timer"_q },
		.icon = &st::menuIconTTL,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_auto_delete_button_about,
	},
	{
		.option = &kHideGiftButton,
		.title = tr::lng_serein_hide_gift_button,
		.id = u"serein/compose/hide-gift-button"_q,
		.keywords = { u"gift"_q },
		.icon = &st::menuIconGiftPremium,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_gift_button_about,
	},
	{
		.option = &kHideAiButton,
		.title = tr::lng_serein_hide_ai_button,
		.id = u"serein/compose/hide-ai-button"_q,
		.keywords = { u"AI"_q },
		.icon = &st::menuIconEmojiObjects,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_ai_button_about,
	},
	{
		.option = &kHideSendAsButton,
		.title = tr::lng_serein_hide_send_as_button,
		.id = u"serein/compose/hide-send-as-button"_q,
		.keywords = { u"send as"_q },
		.icon = &st::menuIconProfile,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_send_as_button_about,
	},
	{
		.option = &kHideStarsReactionButton,
		.title = tr::lng_serein_hide_stars_reaction_button,
		.id = u"serein/compose/hide-stars-reaction-button"_q,
		.keywords = { u"Stars"_q },
		.icon = &st::menuIconStar,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_stars_reaction_button_about,
	},
	{
		.option = &kHideChannelMuteButton,
		.title = tr::lng_serein_hide_channel_mute_button,
		.id = u"serein/compose/hide-channel-mute-button"_q,
		.keywords = { u"channel"_q, u"mute"_q },
		.icon = &st::menuIconMute,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_channel_mute_button_about,
	},
	{
		.option = &kDisableEmojiHover,
		.title = tr::lng_serein_disable_emoji_hover,
		.id = u"serein/compose/disable-emoji-hover"_q,
		.keywords = { u"emoji"_q, u"hover"_q },
		.icon = &st::menuIconStickerAdd,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_disable_emoji_hover_about,
	},
	{
		.option = &kDisableAttachHover,
		.title = tr::lng_serein_disable_attach_hover,
		.id = u"serein/compose/disable-attach-hover"_q,
		.keywords = { u"attachment"_q, u"hover"_q },
		.icon = &st::menuIconPhoto,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_disable_attach_hover_about,
	},
	{
		.option = &kBotCommandsToDraft,
		.title = tr::lng_serein_bot_commands_to_draft,
		.id = u"serein/compose/bot-commands-to-draft"_q,
		.keywords = { u"bot"_q, u"command"_q, u"draft"_q },
		.icon = &st::menuIconShortcut,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_bot_commands_to_draft_about,
	},
	{
		.option = &kMentionMenu,
		.title = tr::lng_serein_mention_menu,
		.id = u"serein/compose/mention-menu"_q,
		.keywords = { u"mention"_q, u"format"_q },
		.icon = &st::menuIconUsername,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_mention_menu_about,
	},
	{
		.option = &kDisableAutoMarkdown,
		.title = tr::lng_serein_disable_auto_markdown,
		.id = u"serein/compose/disable-auto-markdown"_q,
		.keywords = { u"Markdown"_q },
		.icon = &st::menuIconArticle,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_disable_auto_markdown_about,
	},
	{
		.option = &kFormatToolbar,
		.title = tr::lng_serein_format_toolbar,
		.id = u"serein/compose/format-toolbar"_q,
		.keywords = { u"format"_q, u"toolbar"_q, u"bold"_q, u"selection"_q },
		.icon = &st::menuIconFont,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_format_toolbar_about,
	},
	{
		.option = &kSpaceOnSend,
		.title = tr::lng_serein_space_on_send,
		.id = u"serein/compose/space-on-send"_q,
		.keywords = { u"spacing"_q, u"send"_q },
		.icon = &st::menuIconSend,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_space_on_send_about,
	},
	{
		.option = &kSpaceOnEdit,
		.title = tr::lng_serein_space_on_edit,
		.id = u"serein/compose/space-on-edit"_q,
		.keywords = { u"spacing"_q, u"edit"_q },
		.icon = &st::menuIconSigned,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_space_on_edit_about,
	},
	{
		.option = &kDraftTranslation,
		.title = tr::lng_serein_draft_translation,
		.id = u"serein/compose/draft-translation"_q,
		.keywords = { u"translate"_q, u"draft"_q, u"message field"_q },
		.icon = &st::menuIconTranslate,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_draft_translation_about,
	},
	{
		.option = &kDisableLinkPreview,
		.title = tr::lng_serein_disable_link_preview,
		.id = u"serein/compose/disable-link-preview"_q,
		.keywords = { u"link"_q, u"preview"_q },
		.icon = &st::menuIconLink,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_disable_link_preview_about,
	},
	{
		.option = &kCaptionAboveMedia,
		.title = tr::lng_serein_caption_above_media,
		.id = u"serein/compose/caption-above-media"_q,
		.keywords = { u"caption"_q, u"above"_q, u"media"_q, u"photo"_q, u"video"_q },
		.icon = &st::menuIconAbove,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_caption_above_media_about,
	},
	{
		.option = &kSendSilently,
		.title = tr::lng_serein_send_silently,
		.id = u"serein/compose/send-silently"_q,
		.keywords = { u"silent"_q, u"notification"_q, u"send"_q },
		.icon = &st::menuIconSilent,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_send_silently_about,
	},
	{
		.option = &kOwnerSendAs,
		.title = tr::lng_serein_owner_send_as,
		.id = u"serein/compose/owner-send-as"_q,
		.keywords = { u"anonymous"_q, u"send as"_q, u"owner"_q },
		.icon = &st::menuIconStealth,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_owner_send_as_about,
	},
	{
		.option = &kConfirmSticker,
		.title = tr::lng_serein_confirm_sticker,
		.id = u"serein/compose/confirm-sticker"_q,
		.keywords = { u"sticker"_q, u"confirm"_q },
		.icon = &st::menuIconStickers,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_confirm_sticker_about,
	},
	{
		.option = &kConfirmGif,
		.title = tr::lng_serein_confirm_gif,
		.id = u"serein/compose/confirm-gif"_q,
		.keywords = { u"GIF"_q, u"confirm"_q },
		.icon = &st::menuIconGif,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_confirm_gif_about,
	},
	{
		.option = &kPreviewVoice,
		.title = tr::lng_serein_preview_voice,
		.id = u"serein/compose/preview-voice"_q,
		.keywords = { u"voice"_q, u"listen"_q },
		.icon = &st::menuIconVideoChat,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_preview_voice_about,
	},
	{
		.option = &kPreviewRoundVideo,
		.title = tr::lng_serein_preview_round_video,
		.id = u"serein/compose/preview-round-video"_q,
		.keywords = { u"video"_q, u"preview"_q },
		.icon = &st::menuIconStartStream,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_preview_round_video_about,
	},
	{
		.option = &kConfirmPrivateCall,
		.title = tr::lng_serein_confirm_private_call,
		.id = u"serein/compose/confirm-private-call"_q,
		.keywords = { u"call"_q, u"confirm"_q },
		.icon = &st::menuIconPhone,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_confirm_private_call_about,
	},
	{
		.option = &kForwardBeforeComment,
		.title = tr::lng_serein_forward_before_comment,
		.id = u"serein/compose/forward-before-comment"_q,
		.keywords = { u"forward"_q, u"comment"_q, u"order"_q },
		.icon = &st::menuIconReorder,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_forward_before_comment_about,
	},
	{
		.option = &kRememberForwardOptions,
		.title = tr::lng_serein_remember_forward_options,
		.id = u"serein/compose/remember-forward-options"_q,
		.keywords = { u"forward"_q, u"sender"_q, u"names"_q, u"captions"_q, u"remember"_q },
		.icon = &st::menuIconForward,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_remember_forward_options_about,
	},
} };

struct CustomRows {
	CustomRow defaultCodeLanguage;
	CustomRow quickReplies;
	CustomRow textReplacements;
	CustomRow linkInlineBots;
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
	EndSection(builder);
	AddSection(builder, {
		u"serein/compose/input-behavior"_q,
		tr::lng_serein_input_behavior,
		{ u"input"_q, u"behavior"_q },
	});
	AddToggle(builder, kToggleRows[11]);
	AddToggle(builder, kToggleRows[12]);
	AddToggle(builder, kToggleRows[13]);
	AddToggle(builder, kToggleRows[14]);
	AddChoice(builder, {
		.option = &kInputPlaceholderMode,
		.title = tr::lng_serein_input_placeholder,
		.id = u"serein/compose/input-placeholder-mode"_q,
		.keywords = { u"placeholder"_q, u"hint"_q },
		.values = { 0, 1, 2 },
		.labels = { tr::lng_serein_preview_follow, tr::lng_serein_placeholder_chat, tr::lng_serein_placeholder_sender },
		.icon = &st::menuIconAsMessages,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_input_placeholder_mode_about,
	});
	EndSection(builder);
	AddSection(builder, {
		u"serein/compose/text-format"_q,
		tr::lng_serein_text_format,
		{ u"text"_q, u"format"_q },
	});
	AddToggle(builder, kToggleRows[15]);
	AddToggle(builder, kToggleRows[16]);
	custom.defaultCodeLanguage();
	AddToggle(builder, kToggleRows[17]);
	AddToggle(builder, kToggleRows[18]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/compose/writing-tools"_q,
		tr::lng_serein_section_writing_tools,
		{ u"replies"_q, u"replacements"_q, u"translate"_q },
	});
	AddToggle(builder, kToggleRows[19]);
	custom.quickReplies();
	custom.textReplacements();
	custom.linkInlineBots();
	EndSection(builder);
	AddSection(builder, {
		u"serein/compose/sending"_q,
		tr::lng_serein_sending,
		{ u"send"_q },
	});
	AddToggle(builder, kToggleRows[20]);
	AddToggle(builder, kToggleRows[21]);
	AddToggle(builder, kToggleRows[22]);
	AddToggle(builder, kToggleRows[23]);
	EndSection(builder, tr::lng_serein_owner_send_as_note);
	AddSection(builder, {
		u"serein/compose/send-confirmation"_q,
		tr::lng_serein_send_confirmation,
		{ u"send"_q, u"confirm"_q },
	});
	AddToggle(builder, kToggleRows[24]);
	AddToggle(builder, kToggleRows[25]);
	AddToggle(builder, kToggleRows[26]);
	AddToggle(builder, kToggleRows[27]);
	AddToggle(builder, kToggleRows[28]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/compose/forwarding"_q,
		tr::lng_serein_forwarding,
		{ u"forward"_q },
	});
	AddToggle(builder, kToggleRows[29]);
	AddToggle(builder, kToggleRows[30]);
	EndSection(builder, tr::lng_serein_remember_forward_options_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_compose;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_compose_about;
inline const auto kSubpageIcon = &st::menuIconEdit;
inline const auto kSubpageTile = &st::settingsIconBg2;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"compose"_q, u"send"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::Compose
