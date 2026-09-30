// Generated from proto/serein/settings/v1/media.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/media.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/media.h"

namespace Serein::Hooks::Media {

int StickerScale() {
	return ForDevice().Get(Serein::Media::kStickerScale);
}

rpl::producer<int> StickerScaleValue() {
	return ForDevice().Value(Serein::Media::kStickerScale);
}

bool HideStickerTime() {
	return ForDevice().Get(Serein::Media::kHideStickerTime);
}

rpl::producer<bool> HideStickerTimeValue() {
	return ForDevice().Value(Serein::Media::kHideStickerTime);
}

int RecentStickerLimit() {
	return ForDevice().Get(Serein::Media::kRecentStickerLimit);
}

rpl::producer<int> RecentStickerLimitValue() {
	return ForDevice().Value(Serein::Media::kRecentStickerLimit);
}

bool HideGroupStickers() {
	return ForDevice().Get(Serein::Media::kHideGroupStickers);
}

rpl::producer<bool> HideGroupStickersValue() {
	return ForDevice().Value(Serein::Media::kHideGroupStickers);
}

bool HideRecommendedStickers() {
	return ForDevice().Get(Serein::Media::kHideRecommendedStickers);
}

rpl::producer<bool> HideRecommendedStickersValue() {
	return ForDevice().Value(Serein::Media::kHideRecommendedStickers);
}

bool HideRecommendedEmoji() {
	return ForDevice().Get(Serein::Media::kHideRecommendedEmoji);
}

rpl::producer<bool> HideRecommendedEmojiValue() {
	return ForDevice().Value(Serein::Media::kHideRecommendedEmoji);
}

bool HideGifCategories() {
	return ForDevice().Get(Serein::Media::kHideGifCategories);
}

rpl::producer<bool> HideGifCategoriesValue() {
	return ForDevice().Value(Serein::Media::kHideGifCategories);
}

bool HideGreetingSticker() {
	return ForDevice().Get(Serein::Media::kHideGreetingSticker);
}

rpl::producer<bool> HideGreetingStickerValue() {
	return ForDevice().Value(Serein::Media::kHideGreetingSticker);
}

bool DisableVideoAutoplay() {
	return ForDevice().Get(Serein::Media::kDisableVideoAutoplay);
}

rpl::producer<bool> DisableVideoAutoplayValue() {
	return ForDevice().Value(Serein::Media::kDisableVideoAutoplay);
}

bool GifPlaybackControls() {
	return ForDevice().Get(Serein::Media::kGifPlaybackControls);
}

rpl::producer<bool> GifPlaybackControlsValue() {
	return ForDevice().Value(Serein::Media::kGifPlaybackControls);
}

bool Mp4FilePreview() {
	return ForDevice().Get(Serein::Media::kMp4FilePreview);
}

rpl::producer<bool> Mp4FilePreviewValue() {
	return ForDevice().Value(Serein::Media::kMp4FilePreview);
}

} // namespace Serein::Hooks::Media
