// Generated from proto/serein/settings/v1/compose.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <rpl/producer.h>

namespace Serein::Hooks::Compose {

[[nodiscard]] bool HideAttachButton();
[[nodiscard]] rpl::producer<bool> HideAttachButtonValue();
[[nodiscard]] bool HideEmojiButton();
[[nodiscard]] rpl::producer<bool> HideEmojiButtonValue();
[[nodiscard]] bool HideRecordingButton();
[[nodiscard]] rpl::producer<bool> HideRecordingButtonValue();
[[nodiscard]] bool HideBotCommandButton();
[[nodiscard]] rpl::producer<bool> HideBotCommandButtonValue();
[[nodiscard]] bool HideBotMenu();
[[nodiscard]] rpl::producer<bool> HideBotMenuValue();
[[nodiscard]] bool HideAutoDeleteButton();
[[nodiscard]] rpl::producer<bool> HideAutoDeleteButtonValue();
[[nodiscard]] bool HideGiftButton();
[[nodiscard]] rpl::producer<bool> HideGiftButtonValue();
[[nodiscard]] bool HideAiButton();
[[nodiscard]] rpl::producer<bool> HideAiButtonValue();
[[nodiscard]] bool HideSendAsButton();
[[nodiscard]] rpl::producer<bool> HideSendAsButtonValue();
[[nodiscard]] bool HideStarsReactionButton();
[[nodiscard]] rpl::producer<bool> HideStarsReactionButtonValue();
[[nodiscard]] bool HideChannelMuteButton();
[[nodiscard]] rpl::producer<bool> HideChannelMuteButtonValue();
[[nodiscard]] bool DisableEmojiHover();
[[nodiscard]] rpl::producer<bool> DisableEmojiHoverValue();
[[nodiscard]] bool DisableAttachHover();
[[nodiscard]] rpl::producer<bool> DisableAttachHoverValue();
[[nodiscard]] bool BotCommandsToDraft();
[[nodiscard]] rpl::producer<bool> BotCommandsToDraftValue();
[[nodiscard]] int InputPlaceholderMode();
[[nodiscard]] rpl::producer<int> InputPlaceholderModeValue();
[[nodiscard]] bool DisableAutoMarkdown();
[[nodiscard]] rpl::producer<bool> DisableAutoMarkdownValue();
[[nodiscard]] bool DisableLinkPreview();
[[nodiscard]] rpl::producer<bool> DisableLinkPreviewValue();
[[nodiscard]] bool SpaceOnSend();
[[nodiscard]] rpl::producer<bool> SpaceOnSendValue();
[[nodiscard]] bool SpaceOnEdit();
[[nodiscard]] rpl::producer<bool> SpaceOnEditValue();
[[nodiscard]] bool MentionMenu();
[[nodiscard]] rpl::producer<bool> MentionMenuValue();
[[nodiscard]] bool RememberForwardOptions();
[[nodiscard]] rpl::producer<bool> RememberForwardOptionsValue();
[[nodiscard]] int LastForwardOptions();
[[nodiscard]] rpl::producer<int> LastForwardOptionsValue();
[[nodiscard]] bool DraftTranslation();
[[nodiscard]] rpl::producer<bool> DraftTranslationValue();
[[nodiscard]] bool FormatToolbar();
[[nodiscard]] rpl::producer<bool> FormatToolbarValue();
[[nodiscard]] QString DefaultCodeLanguage();
[[nodiscard]] rpl::producer<QString> DefaultCodeLanguageValue();
[[nodiscard]] QByteArray QuickReplies();
[[nodiscard]] rpl::producer<QByteArray> QuickRepliesValue();
[[nodiscard]] QByteArray TextReplacements();
[[nodiscard]] rpl::producer<QByteArray> TextReplacementsValue();
[[nodiscard]] bool ConfirmSticker();
[[nodiscard]] rpl::producer<bool> ConfirmStickerValue();
[[nodiscard]] bool ConfirmGif();
[[nodiscard]] rpl::producer<bool> ConfirmGifValue();
[[nodiscard]] bool PreviewVoice();
[[nodiscard]] rpl::producer<bool> PreviewVoiceValue();
[[nodiscard]] bool PreviewRoundVideo();
[[nodiscard]] rpl::producer<bool> PreviewRoundVideoValue();
[[nodiscard]] bool ConfirmPrivateCall();
[[nodiscard]] rpl::producer<bool> ConfirmPrivateCallValue();
[[nodiscard]] bool ForwardBeforeComment();
[[nodiscard]] rpl::producer<bool> ForwardBeforeCommentValue();
[[nodiscard]] bool SendSilently();
[[nodiscard]] rpl::producer<bool> SendSilentlyValue();

} // namespace Serein::Hooks::Compose
