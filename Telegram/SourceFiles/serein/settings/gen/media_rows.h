// Generated from proto/serein/settings/v1/media.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/media.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Media {

inline const auto kToggleRows = std::array<ToggleRow, 15>{ {
	{
		.option = &kHideStickerTime,
		.title = tr::lng_serein_hide_sticker_time,
		.id = u"serein/media/hide-sticker-time"_q,
		.keywords = { u"sticker"_q, u"time"_q },
		.icon = &st::menuIconTimer,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_hide_sticker_time_about,
	},
	{
		.option = &kRoundedStickers,
		.title = tr::lng_serein_rounded_stickers,
		.id = u"serein/media/rounded-stickers"_q,
		.keywords = { u"sticker"_q, u"rounded"_q, u"corners"_q },
		.icon = &st::menuIconStickers,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_rounded_stickers_about,
	},
	{
		.option = &kHideGroupStickers,
		.title = tr::lng_serein_hide_group_stickers,
		.id = u"serein/media/hide-group-stickers"_q,
		.keywords = { u"group"_q, u"sticker"_q },
		.icon = &st::menuIconGroupsHide,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_hide_group_stickers_about,
	},
	{
		.option = &kHideRecommendedStickers,
		.title = tr::lng_serein_hide_recommended_stickers,
		.id = u"serein/media/hide-recommended-stickers"_q,
		.keywords = { u"recommended"_q, u"sticker"_q },
		.icon = &st::menuIconStickerAdd,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_hide_recommended_stickers_about,
	},
	{
		.option = &kHideRecommendedEmoji,
		.title = tr::lng_serein_hide_recommended_emoji,
		.id = u"serein/media/hide-recommended-emoji"_q,
		.keywords = { u"recommended"_q, u"emoji"_q },
		.icon = &st::menuIconEmoji,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_hide_recommended_emoji_about,
	},
	{
		.option = &kHideGreetingSticker,
		.title = tr::lng_serein_hide_greeting_sticker,
		.id = u"serein/media/hide-greeting-sticker"_q,
		.keywords = { u"greeting"_q, u"sticker"_q },
		.icon = &st::menuIconWelcomeMessage,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_hide_greeting_sticker_about,
	},
	{
		.option = &kStickerPackAuthor,
		.title = tr::lng_serein_sticker_pack_author,
		.id = u"serein/media/sticker-pack-author"_q,
		.keywords = { u"sticker"_q, u"emoji"_q, u"pack"_q, u"author"_q, u"creator"_q },
		.icon = &st::menuIconProfile,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_sticker_pack_author_about,
	},
	{
		.option = &kHideGifCategories,
		.title = tr::lng_serein_hide_gif_categories,
		.id = u"serein/media/hide-gif-categories"_q,
		.keywords = { u"GIF"_q, u"categories"_q },
		.icon = &st::menuIconGif,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_hide_gif_categories_about,
	},
	{
		.option = &kDisableVideoAutoplay,
		.title = tr::lng_serein_disable_video_autoplay,
		.id = u"serein/media/disable-video-autoplay"_q,
		.keywords = { u"video"_q, u"autoplay"_q },
		.icon = &st::menuIconStartStream,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_disable_video_autoplay_about,
	},
	{
		.option = &kGifPlaybackControls,
		.title = tr::lng_serein_gif_playback_controls,
		.id = u"serein/media/gif-playback-controls"_q,
		.keywords = { u"GIF"_q, u"playback"_q, u"controls"_q },
		.icon = &st::menuIconCustomize,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_gif_playback_controls_about,
	},
	{
		.option = &kMp4FilePreview,
		.title = tr::lng_serein_mp4_file_preview,
		.id = u"serein/media/mp4-file-preview"_q,
		.keywords = { u"MP4"_q, u"file"_q, u"preview"_q },
		.icon = &st::menuIconFile,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_mp4_file_preview_about,
	},
	{
		.option = &kForceClickPreview,
		.title = tr::lng_serein_force_click_preview,
		.id = u"serein/media/force-click-preview"_q,
		.keywords = { u"Force Click"_q, u"Force Touch"_q, u"trackpad"_q, u"preview"_q, u"haptic"_q },
		.icon = &st::menuIconTouchID,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_force_click_preview_about,
	},
	{
		.option = &kDownloadsPerChat,
		.title = tr::lng_serein_downloads_per_chat,
		.id = u"serein/media/downloads-per-chat"_q,
		.keywords = { u"download"_q, u"folder"_q, u"chat"_q, u"channel"_q, u"organize"_q },
		.icon = &st::menuIconShowInFolder,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_downloads_per_chat_about,
	},
	{
		.option = &kDenoiseVoiceMessages,
		.title = tr::lng_serein_denoise_voice_messages,
		.id = u"serein/media/denoise-voice-messages"_q,
		.keywords = { u"noise"_q, u"voice"_q, u"recording"_q, u"microphone"_q },
		.icon = &st::menuIconVideoChat,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_denoise_voice_messages_about,
	},
	{
		.option = &kStoryPosting,
		.title = tr::lng_serein_story_posting,
		.id = u"serein/media/story-posting"_q,
		.keywords = { u"story"_q, u"stories"_q, u"post"_q, u"publish"_q, u"close friends"_q },
		.icon = &st::menuIconStoriesSavedSection,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_story_posting_about,
	},
} };

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder) {
	AddSection(builder, {
		u"serein/media/stickers"_q,
		tr::lng_serein_section_stickers,
		{ u"sticker"_q, u"emoji"_q },
	});
	AddChoice(builder, {
		.option = &kStickerScale,
		.title = tr::lng_serein_sticker_scale,
		.id = u"serein/media/sticker-scale"_q,
		.keywords = { u"sticker"_q, u"size"_q },
		.values = { 50, 75, 100, 125, 150, 175, 200 },
		.suffix = u"%"_q,
		.icon = &st::menuIconEnlarge,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_sticker_scale_about,
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
		.icon = &st::menuIconReschedule,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_recent_sticker_limit_about,
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
		.icon = &st::menuIconBot,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_sticker_author_bot_about,
	});
	EndSection(builder, tr::lng_serein_sticker_author_bot_note);
	AddSection(builder, {
		u"serein/media/gifs-videos"_q,
		tr::lng_serein_section_gifs_videos,
		{ u"GIF"_q, u"video"_q },
	});
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	EndSection(builder, tr::lng_serein_force_click_preview_note);
	AddSection(builder, {
		u"serein/media/more-media"_q,
		tr::lng_serein_section_more_media,
		{ u"files"_q, u"voice"_q, u"stories"_q },
	});
	AddToggle(builder, kToggleRows[12]);
	AddNote(builder, tr::lng_serein_downloads_per_chat_note);
	AddToggle(builder, kToggleRows[13]);
	AddNote(builder, tr::lng_serein_denoise_voice_messages_note);
	AddToggle(builder, kToggleRows[14]);
	EndSection(builder, tr::lng_serein_story_posting_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_media;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_media_about;
inline const auto kSubpageIcon = &st::menuIconPhoto;
inline const auto kSubpageTile = &st::settingsIconBg3;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"media"_q, u"sticker"_q, u"emoji"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::Media
