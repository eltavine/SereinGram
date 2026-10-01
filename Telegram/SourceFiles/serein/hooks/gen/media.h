// Generated from proto/serein/settings/v1/media.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QString>
#include <rpl/producer.h>

namespace Serein::Hooks::Media {

[[nodiscard]] int StickerScale();
[[nodiscard]] rpl::producer<int> StickerScaleValue();
[[nodiscard]] bool HideStickerTime();
[[nodiscard]] rpl::producer<bool> HideStickerTimeValue();
[[nodiscard]] bool RoundedStickers();
[[nodiscard]] rpl::producer<bool> RoundedStickersValue();
[[nodiscard]] int RecentStickerLimit();
[[nodiscard]] rpl::producer<int> RecentStickerLimitValue();
[[nodiscard]] bool HideGroupStickers();
[[nodiscard]] rpl::producer<bool> HideGroupStickersValue();
[[nodiscard]] bool HideRecommendedStickers();
[[nodiscard]] rpl::producer<bool> HideRecommendedStickersValue();
[[nodiscard]] bool HideRecommendedEmoji();
[[nodiscard]] rpl::producer<bool> HideRecommendedEmojiValue();
[[nodiscard]] bool HideGifCategories();
[[nodiscard]] rpl::producer<bool> HideGifCategoriesValue();
[[nodiscard]] bool HideGreetingSticker();
[[nodiscard]] rpl::producer<bool> HideGreetingStickerValue();
[[nodiscard]] QString StickerAuthorBot();
[[nodiscard]] rpl::producer<QString> StickerAuthorBotValue();
[[nodiscard]] bool DisableVideoAutoplay();
[[nodiscard]] rpl::producer<bool> DisableVideoAutoplayValue();
[[nodiscard]] bool GifPlaybackControls();
[[nodiscard]] rpl::producer<bool> GifPlaybackControlsValue();
[[nodiscard]] bool ForceClickPreview();
[[nodiscard]] rpl::producer<bool> ForceClickPreviewValue();
[[nodiscard]] bool Mp4FilePreview();
[[nodiscard]] rpl::producer<bool> Mp4FilePreviewValue();
[[nodiscard]] bool DenoiseVoiceMessages();
[[nodiscard]] rpl::producer<bool> DenoiseVoiceMessagesValue();

} // namespace Serein::Hooks::Media
