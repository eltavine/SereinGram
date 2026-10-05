// Generated from proto/serein/settings/v1/messages.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/messages.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Messages {

inline const auto kToggleRows = std::array<ToggleRow, 34>{ {
	{
		.option = &kSecondsInMessages,
		.title = tr::lng_serein_seconds_in_messages,
		.id = u"serein/messages/seconds-in-messages"_q,
		.keywords = { u"seconds"_q, u"timestamp"_q },
		.icon = &st::menuIconTimer,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_seconds_in_messages_about,
	},
	{
		.option = &kShowForwardedMessageDate,
		.title = tr::lng_serein_show_forwarded_message_date,
		.id = u"serein/messages/show-forwarded-message-date"_q,
		.keywords = { u"forwarded"_q, u"original time"_q },
		.icon = &st::menuIconForward,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_show_forwarded_message_date_about,
	},
	{
		.option = &kShowServiceTime,
		.title = tr::lng_serein_show_service_time,
		.id = u"serein/messages/show-service-time"_q,
		.keywords = { u"service"_q, u"time"_q },
		.icon = &st::menuIconInfo,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_show_service_time_about,
	},
	{
		.option = &kShowMessageId,
		.title = tr::lng_serein_show_message_id,
		.id = u"serein/messages/show-message-id"_q,
		.keywords = { u"message ID"_q, u"tooltip"_q },
		.icon = &st::menuIconOrderNumber,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_show_message_id_about,
	},
	{
		.option = &kPersianCalendar,
		.title = tr::lng_serein_persian_calendar,
		.id = u"serein/messages/persian-calendar"_q,
		.keywords = { u"Persian"_q, u"Jalali"_q, u"Solar Hijri"_q, u"calendar"_q, u"date"_q },
		.icon = &st::menuIconSchedule,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_persian_calendar_about,
	},
	{
		.option = &kExactMessageCounters,
		.title = tr::lng_serein_exact_message_counters,
		.id = u"serein/messages/exact-message-counters"_q,
		.keywords = { u"exact"_q, u"views"_q, u"replies"_q },
		.icon = &st::menuIconStats,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_exact_message_counters_about,
	},
	{
		.option = &kHideMessageViews,
		.title = tr::lng_serein_hide_message_views,
		.id = u"serein/messages/hide-message-views"_q,
		.keywords = { u"hide"_q, u"views"_q },
		.icon = &st::menuIconStealth,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_hide_message_views_about,
	},
	{
		.option = &kHideChannelSignature,
		.title = tr::lng_serein_hide_channel_signature,
		.id = u"serein/messages/hide-channel-signature"_q,
		.keywords = { u"channel"_q, u"signature"_q },
		.icon = &st::menuIconSigned,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_hide_channel_signature_about,
	},
	{
		.option = &kHideEditedBadge,
		.title = tr::lng_serein_hide_edited_badge,
		.id = u"serein/messages/hide-edited-badge"_q,
		.keywords = { u"edited"_q, u"badge"_q },
		.icon = &st::menuIconCaptionHide,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_hide_edited_badge_about,
	},
	{
		.option = &kFadeDeletedMessages,
		.title = tr::lng_serein_fade_deleted_messages,
		.id = u"serein/messages/fade-deleted-messages"_q,
		.keywords = { u"deleted"_q, u"transparent"_q, u"anti-recall"_q },
		.icon = &st::menuIconTransparent,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_fade_deleted_messages_about,
	},
	{
		.option = &kHighlightHistoryMarks,
		.title = tr::lng_serein_highlight_history_marks,
		.id = u"serein/messages/highlight-history-marks"_q,
		.keywords = { u"deleted"_q, u"edited"_q, u"red"_q, u"icon"_q, u"anti-recall"_q },
		.icon = &st::menuIconChangeColors,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_highlight_history_marks_about,
	},
	{
		.option = &kShowChannelBadge,
		.title = tr::lng_serein_show_channel_badge,
		.id = u"serein/messages/show-channel-badge"_q,
		.keywords = { u"channel"_q, u"badge"_q, u"sender"_q },
		.icon = &st::menuIconAdmin,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_show_channel_badge_about,
	},
	{
		.option = &kHideReactions,
		.title = tr::lng_serein_hide_reactions,
		.id = u"serein/messages/hide-reactions"_q,
		.keywords = { u"hide"_q, u"reactions"_q },
		.icon = &st::menuIconReactions,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_reactions_about,
	},
	{
		.option = &kHidePrivateReactions,
		.title = tr::lng_serein_hide_private_reactions,
		.id = u"serein/messages/hide-private-reactions"_q,
		.keywords = { u"private"_q, u"chat reactions"_q },
		.disabledBy = &kHideReactions,
		.icon = &st::menuIconProfile,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_private_reactions_about,
	},
	{
		.option = &kHideGroupReactions,
		.title = tr::lng_serein_hide_group_reactions,
		.id = u"serein/messages/hide-group-reactions"_q,
		.keywords = { u"group"_q, u"chat reactions"_q },
		.disabledBy = &kHideReactions,
		.icon = &st::menuIconGroups,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_group_reactions_about,
	},
	{
		.option = &kHideChannelReactions,
		.title = tr::lng_serein_hide_channel_reactions,
		.id = u"serein/messages/hide-channel-reactions"_q,
		.keywords = { u"channel"_q, u"chat reactions"_q },
		.disabledBy = &kHideReactions,
		.icon = &st::menuIconChannel,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_channel_reactions_about,
	},
	{
		.option = &kHideReactionMenu,
		.title = tr::lng_serein_hide_reaction_menu,
		.id = u"serein/messages/hide-reaction-menu"_q,
		.keywords = { u"context menu"_q, u"reaction panel"_q },
		.icon = &st::menuIconGroupReactions,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_reaction_menu_about,
	},
	{
		.option = &kHideReactionMenuWhenSelecting,
		.title = tr::lng_serein_hide_reaction_menu_when_selecting,
		.id = u"serein/messages/hide-reaction-menu-when-selecting"_q,
		.keywords = { u"selection"_q, u"reaction panel"_q },
		.icon = &st::menuIconMarkRead,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_hide_reaction_menu_when_selecting_about,
	},
	{
		.option = &kDisablePremiumStickerEffects,
		.title = tr::lng_serein_disable_premium_sticker_effects,
		.id = u"serein/messages/disable-premium-sticker-effects"_q,
		.keywords = { u"Premium"_q, u"sticker effects"_q },
		.icon = &st::menuIconStickers,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_disable_premium_sticker_effects_about,
	},
	{
		.option = &kDisableEmojiInteractions,
		.title = tr::lng_serein_disable_emoji_interactions,
		.id = u"serein/messages/disable-emoji-interactions"_q,
		.keywords = { u"emoji"_q, u"interactions"_q },
		.icon = &st::menuIconEmoji,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_disable_emoji_interactions_about,
	},
	{
		.option = &kDisableMessageEffects,
		.title = tr::lng_serein_disable_message_effects,
		.id = u"serein/messages/disable-message-effects"_q,
		.keywords = { u"message effects"_q },
		.icon = &st::menuIconUnique,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_disable_message_effects_about,
	},
	{
		.option = &kRevealSpoilers,
		.title = tr::lng_serein_reveal_spoilers,
		.id = u"serein/messages/reveal-spoilers"_q,
		.keywords = { u"reveal"_q, u"spoilers"_q },
		.icon = &st::menuIconSpoiler,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_reveal_spoilers_about,
	},
	{
		.option = &kHideQuickShare,
		.title = tr::lng_serein_hide_quick_share,
		.id = u"serein/messages/hide-quick-share"_q,
		.keywords = { u"quick forward"_q, u"share"_q },
		.icon = &st::menuIconShareOff,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_quick_share_about,
	},
	{
		.option = &kHideRecommendedChannels,
		.title = tr::lng_serein_hide_recommended_channels,
		.id = u"serein/messages/hide-recommended-channels"_q,
		.keywords = { u"recommended"_q, u"channels"_q },
		.icon = &st::menuIconShowAll,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_recommended_channels_about,
	},
	{
		.option = &kHidePremiumBadges,
		.title = tr::lng_serein_hide_premium_badges,
		.id = u"serein/messages/hide-premium-badges"_q,
		.keywords = { u"Premium"_q, u"emoji status"_q },
		.icon = &st::menuIconPremium,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_premium_badges_about,
	},
	{
		.option = &kHideSavedTags,
		.title = tr::lng_serein_hide_saved_tags,
		.id = u"serein/messages/hide-saved-tags"_q,
		.keywords = { u"saved messages"_q, u"tags"_q },
		.icon = &st::menuIconSavedMessages,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_saved_tags_about,
	},
	{
		.option = &kHidePrivateChatActivities,
		.title = tr::lng_serein_hide_private_chat_activities,
		.id = u"serein/messages/hide-private-chat-activities"_q,
		.keywords = { u"typing"_q, u"recording"_q, u"private chat"_q },
		.icon = &st::menuIconDiscussion,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_hide_private_chat_activities_about,
	},
	{
		.option = &kReadingSpacing,
		.title = tr::lng_serein_reading_spacing,
		.id = u"serein/messages/reading-spacing"_q,
		.keywords = { u"reading"_q, u"spacing"_q },
		.icon = &st::menuIconFont,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_reading_spacing_about,
	},
	{
		.option = &kRaiseSelectionLimit,
		.title = tr::lng_serein_raise_selection_limit,
		.id = u"serein/messages/raise-selection-limit"_q,
		.keywords = { u"select"_q, u"selection"_q, u"limit"_q, u"forward"_q, u"delete"_q, u"batch"_q },
		.icon = &st::menuIconSelect,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_raise_selection_limit_about,
	},
	{
		.option = &kDoubleClickEditsOwn,
		.title = tr::lng_serein_double_click_edits_own,
		.id = u"serein/messages/double-click-edits-own"_q,
		.keywords = { u"double click"_q, u"edit"_q, u"own messages"_q },
		.icon = &st::menuIconEdit,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_double_click_edits_own_about,
	},
	{
		.option = &kRevokePrivateChatDeletion,
		.title = tr::lng_serein_revoke_private_chat_deletion,
		.id = u"serein/messages/revoke-private-chat-deletion"_q,
		.keywords = { u"delete for everyone"_q, u"clear history"_q, u"private chat"_q },
		.icon = &st::menuIconClear,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_revoke_private_chat_deletion_about,
	},
	{
		.option = &kModerateReportSpam,
		.title = tr::lng_serein_moderate_report_spam,
		.id = u"serein/messages/moderate-report-spam"_q,
		.keywords = { u"report spam"_q, u"moderate"_q },
		.icon = &st::menuIconReport,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_moderate_report_spam_about,
	},
	{
		.option = &kModerateDeleteAll,
		.title = tr::lng_serein_moderate_delete_all,
		.id = u"serein/messages/moderate-delete-all"_q,
		.keywords = { u"delete all"_q, u"moderate"_q },
		.icon = &st::menuIconDelete,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_moderate_delete_all_about,
	},
	{
		.option = &kModerateBan,
		.title = tr::lng_serein_moderate_ban,
		.id = u"serein/messages/moderate-ban"_q,
		.keywords = { u"ban"_q, u"moderate"_q },
		.icon = &st::menuIconBlock,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_moderate_ban_about,
	},
} };

struct CustomRows {
	CustomRow readingChinese;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddSection(builder, {
		u"serein/messages/time"_q,
		tr::lng_serein_time_and_info,
		{ u"time"_q, u"info"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	EndSection(builder, tr::lng_serein_persian_calendar_note);
	AddSection(builder, {
		u"serein/messages/marks"_q,
		tr::lng_serein_marks_and_counts,
		{ u"labels"_q, u"counts"_q },
	});
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddText(builder, {
		.option = &kEditedMark,
		.title = tr::lng_serein_edited_mark,
		.id = u"serein/messages/edited-mark"_q,
		.keywords = { u"edited"_q, u"label"_q, u"text"_q },
		.placeholder = tr::lng_edited,
		.hiddenBy = &kHideEditedBadge,
		.icon = &st::menuIconTagEdit,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_edited_mark_about,
	});
	AddText(builder, {
		.option = &kDeletedMark,
		.title = tr::lng_serein_deleted_mark_title,
		.id = u"serein/messages/deleted-mark"_q,
		.keywords = { u"deleted"_q, u"label"_q, u"text"_q, u"anti-recall"_q },
		.placeholder = tr::lng_serein_deleted_mark,
		.icon = &st::menuIconTagRemove,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_deleted_mark_about,
	});
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/messages/reactions"_q,
		tr::lng_serein_reactions,
		{ u"reactions"_q },
	});
	AddToggle(builder, kToggleRows[12]);
	AddToggle(builder, kToggleRows[13]);
	AddToggle(builder, kToggleRows[14]);
	AddToggle(builder, kToggleRows[15]);
	AddToggle(builder, kToggleRows[16]);
	AddToggle(builder, kToggleRows[17]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/messages/effects"_q,
		tr::lng_serein_effects,
		{ u"effects"_q, u"animation"_q },
	});
	AddToggle(builder, kToggleRows[18]);
	AddToggle(builder, kToggleRows[19]);
	AddToggle(builder, kToggleRows[20]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/messages/content"_q,
		tr::lng_serein_content_display,
		{ u"content"_q, u"display"_q },
	});
	AddToggle(builder, kToggleRows[21]);
	AddNote(builder, tr::lng_serein_reveal_spoilers_note);
	AddToggle(builder, kToggleRows[22]);
	AddToggle(builder, kToggleRows[23]);
	AddToggle(builder, kToggleRows[24]);
	AddToggle(builder, kToggleRows[25]);
	AddToggle(builder, kToggleRows[26]);
	EndSection(builder, tr::lng_serein_private_activities_note);
	AddSection(builder, {
		u"serein/messages/reading"_q,
		tr::lng_serein_section_reading,
		{ u"reading"_q, u"Chinese"_q, u"spacing"_q },
	});
	AddToggle(builder, kToggleRows[27]);
	custom.readingChinese();
	EndSection(builder, tr::lng_serein_reading_chinese_note);
	AddSection(builder, {
		u"serein/messages/interaction"_q,
		tr::lng_serein_interaction,
		{ u"select"_q, u"double click"_q },
	});
	AddToggle(builder, kToggleRows[28]);
	AddNote(builder, tr::lng_serein_raise_selection_limit_note);
	AddToggle(builder, kToggleRows[29]);
	EndSection(builder, tr::lng_serein_double_click_edits_own_note);
	AddSection(builder, {
		u"serein/messages/deleting"_q,
		tr::lng_serein_deleting,
		{ u"delete"_q, u"moderate"_q },
	});
	AddToggle(builder, kToggleRows[30]);
	AddToggle(builder, kToggleRows[31]);
	AddToggle(builder, kToggleRows[32]);
	AddToggle(builder, kToggleRows[33]);
	EndSection(builder, tr::lng_serein_delete_defaults_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_messages;
inline const auto kSubpageIcon = &st::menuIconChatBubble;
inline const auto kSubpageTile = &st::settingsIconBg3;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"messages"_q, u"time"_q },
	});
}

} // namespace Serein::Messages
