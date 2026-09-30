// Generated from proto/serein/settings/v1/compose.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/compose.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/compose.h"

namespace Serein::Hooks::Compose {

bool HideAttachButton() {
	return ForDevice().Get(Serein::Compose::kHideAttachButton);
}

rpl::producer<bool> HideAttachButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideAttachButton);
}

bool HideEmojiButton() {
	return ForDevice().Get(Serein::Compose::kHideEmojiButton);
}

rpl::producer<bool> HideEmojiButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideEmojiButton);
}

bool HideRecordingButton() {
	return ForDevice().Get(Serein::Compose::kHideRecordingButton);
}

rpl::producer<bool> HideRecordingButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideRecordingButton);
}

bool HideBotCommandButton() {
	return ForDevice().Get(Serein::Compose::kHideBotCommandButton);
}

rpl::producer<bool> HideBotCommandButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideBotCommandButton);
}

bool HideBotMenu() {
	return ForDevice().Get(Serein::Compose::kHideBotMenu);
}

rpl::producer<bool> HideBotMenuValue() {
	return ForDevice().Value(Serein::Compose::kHideBotMenu);
}

bool HideAutoDeleteButton() {
	return ForDevice().Get(Serein::Compose::kHideAutoDeleteButton);
}

rpl::producer<bool> HideAutoDeleteButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideAutoDeleteButton);
}

bool HideGiftButton() {
	return ForDevice().Get(Serein::Compose::kHideGiftButton);
}

rpl::producer<bool> HideGiftButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideGiftButton);
}

bool HideAiButton() {
	return ForDevice().Get(Serein::Compose::kHideAiButton);
}

rpl::producer<bool> HideAiButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideAiButton);
}

bool HideSendAsButton() {
	return ForDevice().Get(Serein::Compose::kHideSendAsButton);
}

rpl::producer<bool> HideSendAsButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideSendAsButton);
}

bool HideStarsReactionButton() {
	return ForDevice().Get(Serein::Compose::kHideStarsReactionButton);
}

rpl::producer<bool> HideStarsReactionButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideStarsReactionButton);
}

bool HideChannelMuteButton() {
	return ForDevice().Get(Serein::Compose::kHideChannelMuteButton);
}

rpl::producer<bool> HideChannelMuteButtonValue() {
	return ForDevice().Value(Serein::Compose::kHideChannelMuteButton);
}

bool DisableEmojiHover() {
	return ForDevice().Get(Serein::Compose::kDisableEmojiHover);
}

rpl::producer<bool> DisableEmojiHoverValue() {
	return ForDevice().Value(Serein::Compose::kDisableEmojiHover);
}

bool DisableAttachHover() {
	return ForDevice().Get(Serein::Compose::kDisableAttachHover);
}

rpl::producer<bool> DisableAttachHoverValue() {
	return ForDevice().Value(Serein::Compose::kDisableAttachHover);
}

bool BotCommandsToDraft() {
	return ForDevice().Get(Serein::Compose::kBotCommandsToDraft);
}

rpl::producer<bool> BotCommandsToDraftValue() {
	return ForDevice().Value(Serein::Compose::kBotCommandsToDraft);
}

int InputPlaceholderMode() {
	return ForDevice().Get(Serein::Compose::kInputPlaceholderMode);
}

rpl::producer<int> InputPlaceholderModeValue() {
	return ForDevice().Value(Serein::Compose::kInputPlaceholderMode);
}

bool DisableAutoMarkdown() {
	return ForDevice().Get(Serein::Compose::kDisableAutoMarkdown);
}

rpl::producer<bool> DisableAutoMarkdownValue() {
	return ForDevice().Value(Serein::Compose::kDisableAutoMarkdown);
}

bool DisableLinkPreview() {
	return ForDevice().Get(Serein::Compose::kDisableLinkPreview);
}

rpl::producer<bool> DisableLinkPreviewValue() {
	return ForDevice().Value(Serein::Compose::kDisableLinkPreview);
}

bool SpaceOnSend() {
	return ForDevice().Get(Serein::Compose::kSpaceOnSend);
}

rpl::producer<bool> SpaceOnSendValue() {
	return ForDevice().Value(Serein::Compose::kSpaceOnSend);
}

bool SpaceOnEdit() {
	return ForDevice().Get(Serein::Compose::kSpaceOnEdit);
}

rpl::producer<bool> SpaceOnEditValue() {
	return ForDevice().Value(Serein::Compose::kSpaceOnEdit);
}

bool MentionMenu() {
	return ForDevice().Get(Serein::Compose::kMentionMenu);
}

rpl::producer<bool> MentionMenuValue() {
	return ForDevice().Value(Serein::Compose::kMentionMenu);
}

QString DefaultCodeLanguage() {
	return ForDevice().Get(Serein::Compose::kDefaultCodeLanguage);
}

rpl::producer<QString> DefaultCodeLanguageValue() {
	return ForDevice().Value(Serein::Compose::kDefaultCodeLanguage);
}

QByteArray QuickReplies() {
	return ForDevice().Get(Serein::Compose::kQuickReplies);
}

rpl::producer<QByteArray> QuickRepliesValue() {
	return ForDevice().Value(Serein::Compose::kQuickReplies);
}

bool ConfirmSticker() {
	return ForDevice().Get(Serein::Compose::kConfirmSticker);
}

rpl::producer<bool> ConfirmStickerValue() {
	return ForDevice().Value(Serein::Compose::kConfirmSticker);
}

bool ConfirmGif() {
	return ForDevice().Get(Serein::Compose::kConfirmGif);
}

rpl::producer<bool> ConfirmGifValue() {
	return ForDevice().Value(Serein::Compose::kConfirmGif);
}

bool PreviewVoice() {
	return ForDevice().Get(Serein::Compose::kPreviewVoice);
}

rpl::producer<bool> PreviewVoiceValue() {
	return ForDevice().Value(Serein::Compose::kPreviewVoice);
}

bool PreviewRoundVideo() {
	return ForDevice().Get(Serein::Compose::kPreviewRoundVideo);
}

rpl::producer<bool> PreviewRoundVideoValue() {
	return ForDevice().Value(Serein::Compose::kPreviewRoundVideo);
}

bool ConfirmPrivateCall() {
	return ForDevice().Get(Serein::Compose::kConfirmPrivateCall);
}

rpl::producer<bool> ConfirmPrivateCallValue() {
	return ForDevice().Value(Serein::Compose::kConfirmPrivateCall);
}

bool ForwardBeforeComment() {
	return ForDevice().Get(Serein::Compose::kForwardBeforeComment);
}

rpl::producer<bool> ForwardBeforeCommentValue() {
	return ForDevice().Value(Serein::Compose::kForwardBeforeComment);
}

bool SendSilently() {
	return ForDevice().Get(Serein::Compose::kSendSilently);
}

rpl::producer<bool> SendSilentlyValue() {
	return ForDevice().Value(Serein::Compose::kSendSilently);
}

} // namespace Serein::Hooks::Compose
