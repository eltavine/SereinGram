// Generated from proto/serein/settings/v1/media.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"
#include "serein/schema/codec.h"

namespace Serein::Media {

inline constexpr auto kStickerScale = Option<int>{
	"serein.stickerScale",
	Scope::Device,
	100,
	Category::Media,
	"lng_serein_sticker_scale",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const int &value) {
		return (value == 100)
			|| (((value == 50 || value == 75 || value == 100 || value == 125 || value == 150 || value == 175 || value == 200)));
	} };
inline constexpr auto kHideStickerTime = Option<bool>{
	"serein.hideStickerTime",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_hide_sticker_time",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kRoundedStickers = Option<bool>{
	"serein.roundedStickers",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_rounded_stickers",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kRecentStickerLimit = Option<int>{
	"serein.recentStickerLimit",
	Scope::Device,
	0,
	Category::Media,
	"lng_serein_recent_sticker_limit",
	0,
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 200));
	} };
inline constexpr auto kHideGroupStickers = Option<bool>{
	"serein.hideGroupStickers",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_hide_group_stickers",
	0 };
inline constexpr auto kHideRecommendedStickers = Option<bool>{
	"serein.hideRecommendedStickers",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_hide_recommended_stickers",
	0 };
inline constexpr auto kHideRecommendedEmoji = Option<bool>{
	"serein.hideRecommendedEmoji",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_hide_recommended_emoji",
	0 };
inline constexpr auto kHideGifCategories = Option<bool>{
	"serein.hideGifCategories",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_hide_gif_categories",
	0 };
inline constexpr auto kHideGreetingSticker = Option<bool>{
	"serein.hideGreetingSticker",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_hide_greeting_sticker",
	0 };
inline constexpr auto kStickerPackAuthor = Option<bool>{
	"serein.stickerPackAuthor",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_sticker_pack_author",
	0 };
inline const auto kStickerAuthorBot = Option<QString>{
	"serein.stickerAuthorBot",
	Scope::Device,
	QString(),
	Category::Media,
	"lng_serein_sticker_author_bot",
	0,
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 33) && (Codec::Matches(value, QString::fromUtf8("^(@?[A-Za-z][A-Za-z0-9_]{3,31})?$"))));
	} };
inline constexpr auto kDisableVideoAutoplay = Option<bool>{
	"serein.disableVideoAutoplay",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_disable_video_autoplay",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kGifPlaybackControls = Option<bool>{
	"serein.gifPlaybackControls",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_gif_playback_controls",
	0 };
inline constexpr auto kForceClickPreview = Option<bool>{
	"serein.forceClickPreview",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_force_click_preview",
	0 };
inline constexpr auto kDownloadsPerChat = Option<bool>{
	"serein.downloadsPerChat",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_downloads_per_chat",
	0 };
inline constexpr auto kMp4FilePreview = Option<bool>{
	"serein.mp4FilePreview",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_mp4_file_preview",
	0 };
inline constexpr auto kDenoiseVoiceMessages = Option<bool>{
	"serein.denoiseVoiceMessages",
	Scope::Device,
	false,
	Category::Media,
	"lng_serein_denoise_voice_messages",
	0 };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kStickerScale));
	Expects(registry.Add(kHideStickerTime));
	Expects(registry.Add(kRoundedStickers));
	Expects(registry.Add(kRecentStickerLimit));
	Expects(registry.Add(kHideGroupStickers));
	Expects(registry.Add(kHideRecommendedStickers));
	Expects(registry.Add(kHideRecommendedEmoji));
	Expects(registry.Add(kHideGifCategories));
	Expects(registry.Add(kHideGreetingSticker));
	Expects(registry.Add(kStickerPackAuthor));
	Expects(registry.Add(kStickerAuthorBot));
	Expects(registry.Add(kDisableVideoAutoplay));
	Expects(registry.Add(kGifPlaybackControls));
	Expects(registry.Add(kForceClickPreview));
	Expects(registry.Add(kDownloadsPerChat));
	Expects(registry.Add(kMp4FilePreview));
	Expects(registry.Add(kDenoiseVoiceMessages));
}

} // namespace Serein::Media
