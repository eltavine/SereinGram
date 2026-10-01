// Generated from proto/serein/settings/v1/media.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/media.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Media {

inline const auto kToggleRows = std::array<ToggleRow, 12>{ {
	{
		&kHideStickerTime,
		tr::lng_serein_hide_sticker_time,
		u"serein/media/hide-sticker-time"_q,
		{ u"sticker"_q, u"time"_q },
	},
	{
		&kRoundedStickers,
		tr::lng_serein_rounded_stickers,
		u"serein/media/rounded-stickers"_q,
		{ u"sticker"_q, u"rounded"_q, u"corners"_q },
	},
	{
		&kHideGroupStickers,
		tr::lng_serein_hide_group_stickers,
		u"serein/media/hide-group-stickers"_q,
		{ u"group"_q, u"sticker"_q },
	},
	{
		&kHideRecommendedStickers,
		tr::lng_serein_hide_recommended_stickers,
		u"serein/media/hide-recommended-stickers"_q,
		{ u"recommended"_q, u"sticker"_q },
	},
	{
		&kHideRecommendedEmoji,
		tr::lng_serein_hide_recommended_emoji,
		u"serein/media/hide-recommended-emoji"_q,
		{ u"recommended"_q, u"emoji"_q },
	},
	{
		&kHideGifCategories,
		tr::lng_serein_hide_gif_categories,
		u"serein/media/hide-gif-categories"_q,
		{ u"GIF"_q, u"categories"_q },
	},
	{
		&kHideGreetingSticker,
		tr::lng_serein_hide_greeting_sticker,
		u"serein/media/hide-greeting-sticker"_q,
		{ u"greeting"_q, u"sticker"_q },
	},
	{
		&kDisableVideoAutoplay,
		tr::lng_serein_disable_video_autoplay,
		u"serein/media/disable-video-autoplay"_q,
		{ u"video"_q, u"autoplay"_q },
	},
	{
		&kGifPlaybackControls,
		tr::lng_serein_gif_playback_controls,
		u"serein/media/gif-playback-controls"_q,
		{ u"GIF"_q, u"playback"_q, u"controls"_q },
	},
	{
		&kForceClickPreview,
		tr::lng_serein_force_click_preview,
		u"serein/media/force-click-preview"_q,
		{ u"Force Click"_q, u"Force Touch"_q, u"trackpad"_q, u"preview"_q, u"haptic"_q },
	},
	{
		&kMp4FilePreview,
		tr::lng_serein_mp4_file_preview,
		u"serein/media/mp4-file-preview"_q,
		{ u"MP4"_q, u"file"_q, u"preview"_q },
	},
	{
		&kDenoiseVoiceMessages,
		tr::lng_serein_denoise_voice_messages,
		u"serein/media/denoise-voice-messages"_q,
		{ u"noise"_q, u"voice"_q, u"recording"_q, u"microphone"_q },
	},
} };

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder) {
	AddChoice(builder, {
		.option = &kStickerScale,
		.title = tr::lng_serein_sticker_scale,
		.id = u"serein/media/sticker-scale"_q,
		.keywords = { u"sticker"_q, u"size"_q },
		.values = { 50, 75, 100, 125, 150, 175, 200 },
		.suffix = u"%"_q,
	});
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddNumber(builder, {
		.option = &kRecentStickerLimit,
		.title = tr::lng_serein_recent_sticker_limit,
		.id = u"serein/media/recent-sticker-limit"_q,
		.keywords = { u"recent"_q, u"sticker"_q, u"limit"_q },
		.minimum = 1,
		.maximum = 200,
		.zeroLabel = tr::lng_serein_preview_follow,
	});
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	AddText(builder, {
		.option = &kStickerAuthorBot,
		.title = tr::lng_serein_sticker_author_bot,
		.id = u"serein/media/sticker-author-bot"_q,
		.keywords = { u"sticker"_q, u"author"_q, u"bot"_q, u"lookup"_q },
		.placeholder = tr::lng_serein_sticker_author_bot_placeholder,
	});
	AddNote(builder, tr::lng_serein_sticker_author_bot_note);
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddNote(builder, tr::lng_serein_force_click_preview_note);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	AddNote(builder, tr::lng_serein_denoise_voice_messages_note);
}

} // namespace Serein::Media
