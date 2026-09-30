#pragma once

#include "serein/core/options.h"

namespace Serein::Media {

inline constexpr auto kStickerScale = Option<int>{
	"serein.stickerScale", Scope::Device, 100,
	Category::Media, "lng_serein_sticker_scale",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const int &value) {
		return value >= 50 && value <= 200 && value % 25 == 0;
	} };
inline constexpr auto kHideStickerTime = Option<bool>{
	"serein.hideStickerTime", Scope::Device, false,
	Category::Media, "lng_serein_hide_sticker_time",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kRecentStickerLimit = Option<int>{
	"serein.recentStickerLimit", Scope::Device, 0,
	Category::Media, "lng_serein_recent_sticker_limit", 0,
	[](const int &value) { return value >= 0 && value <= 200; } };
inline constexpr auto kHideGroupStickers = Option<bool>{
	"serein.hideGroupStickers", Scope::Device, false,
	Category::Media, "lng_serein_hide_group_stickers" };
inline constexpr auto kHideRecommendedStickers = Option<bool>{
	"serein.hideRecommendedStickers", Scope::Device, false,
	Category::Media, "lng_serein_hide_recommended_stickers" };
inline constexpr auto kHideRecommendedEmoji = Option<bool>{
	"serein.hideRecommendedEmoji", Scope::Device, false,
	Category::Media, "lng_serein_hide_recommended_emoji" };
inline constexpr auto kHideGifCategories = Option<bool>{
	"serein.hideGifCategories", Scope::Device, false,
	Category::Media, "lng_serein_hide_gif_categories" };
inline constexpr auto kHideGreetingSticker = Option<bool>{
	"serein.hideGreetingSticker", Scope::Device, false,
	Category::Media, "lng_serein_hide_greeting_sticker" };
inline constexpr auto kDisableVideoAutoplay = Option<bool>{
	"serein.disableVideoAutoplay", Scope::Device, false,
	Category::Media, "lng_serein_disable_video_autoplay",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kGifPlaybackControls = Option<bool>{
	"serein.gifPlaybackControls", Scope::Device, false,
	Category::Media, "lng_serein_gif_playback_controls" };
inline constexpr auto kMp4FilePreview = Option<bool>{
	"serein.mp4FilePreview", Scope::Device, false,
	Category::Media, "lng_serein_mp4_file_preview" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kStickerScale));
	Expects(registry.Add(kHideStickerTime));
	Expects(registry.Add(kRecentStickerLimit));
	Expects(registry.Add(kHideGroupStickers));
	Expects(registry.Add(kHideRecommendedStickers));
	Expects(registry.Add(kHideRecommendedEmoji));
	Expects(registry.Add(kHideGifCategories));
	Expects(registry.Add(kHideGreetingSticker));
	Expects(registry.Add(kDisableVideoAutoplay));
	Expects(registry.Add(kGifPlaybackControls));
	Expects(registry.Add(kMp4FilePreview));
}

} // namespace Serein::Media
