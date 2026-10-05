// Generated from proto/serein/settings/v1/messages.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/messages.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"

#include <array>

namespace Serein::Messages {

inline const auto kToggleRows = std::array<ToggleRow, 34>{ {
	{
		&kSecondsInMessages,
		tr::lng_serein_seconds_in_messages,
		u"serein/messages/seconds-in-messages"_q,
		{ u"seconds"_q, u"timestamp"_q },
	},
	{
		&kShowForwardedMessageDate,
		tr::lng_serein_show_forwarded_message_date,
		u"serein/messages/show-forwarded-message-date"_q,
		{ u"forwarded"_q, u"original time"_q },
	},
	{
		&kShowServiceTime,
		tr::lng_serein_show_service_time,
		u"serein/messages/show-service-time"_q,
		{ u"service"_q, u"time"_q },
	},
	{
		&kShowMessageId,
		tr::lng_serein_show_message_id,
		u"serein/messages/show-message-id"_q,
		{ u"message ID"_q, u"tooltip"_q },
	},
	{
		&kPersianCalendar,
		tr::lng_serein_persian_calendar,
		u"serein/messages/persian-calendar"_q,
		{ u"Persian"_q, u"Jalali"_q, u"Solar Hijri"_q, u"calendar"_q, u"date"_q },
	},
	{
		&kExactMessageCounters,
		tr::lng_serein_exact_message_counters,
		u"serein/messages/exact-message-counters"_q,
		{ u"exact"_q, u"views"_q, u"replies"_q },
	},
	{
		&kHideMessageViews,
		tr::lng_serein_hide_message_views,
		u"serein/messages/hide-message-views"_q,
		{ u"hide"_q, u"views"_q },
	},
	{
		&kHideChannelSignature,
		tr::lng_serein_hide_channel_signature,
		u"serein/messages/hide-channel-signature"_q,
		{ u"channel"_q, u"signature"_q },
	},
	{
		&kHideEditedBadge,
		tr::lng_serein_hide_edited_badge,
		u"serein/messages/hide-edited-badge"_q,
		{ u"edited"_q, u"badge"_q },
	},
	{
		&kFadeDeletedMessages,
		tr::lng_serein_fade_deleted_messages,
		u"serein/messages/fade-deleted-messages"_q,
		{ u"deleted"_q, u"transparent"_q, u"anti-recall"_q },
	},
	{
		&kHighlightHistoryMarks,
		tr::lng_serein_highlight_history_marks,
		u"serein/messages/highlight-history-marks"_q,
		{ u"deleted"_q, u"edited"_q, u"red"_q, u"icon"_q, u"anti-recall"_q },
	},
	{
		&kShowChannelBadge,
		tr::lng_serein_show_channel_badge,
		u"serein/messages/show-channel-badge"_q,
		{ u"channel"_q, u"badge"_q, u"sender"_q },
	},
	{
		&kHideReactions,
		tr::lng_serein_hide_reactions,
		u"serein/messages/hide-reactions"_q,
		{ u"hide"_q, u"reactions"_q },
	},
	{
		&kHidePrivateReactions,
		tr::lng_serein_hide_private_reactions,
		u"serein/messages/hide-private-reactions"_q,
		{ u"private"_q, u"chat reactions"_q },
		&kHideReactions,
	},
	{
		&kHideGroupReactions,
		tr::lng_serein_hide_group_reactions,
		u"serein/messages/hide-group-reactions"_q,
		{ u"group"_q, u"chat reactions"_q },
		&kHideReactions,
	},
	{
		&kHideChannelReactions,
		tr::lng_serein_hide_channel_reactions,
		u"serein/messages/hide-channel-reactions"_q,
		{ u"channel"_q, u"chat reactions"_q },
		&kHideReactions,
	},
	{
		&kHideReactionMenu,
		tr::lng_serein_hide_reaction_menu,
		u"serein/messages/hide-reaction-menu"_q,
		{ u"context menu"_q, u"reaction panel"_q },
	},
	{
		&kHideReactionMenuWhenSelecting,
		tr::lng_serein_hide_reaction_menu_when_selecting,
		u"serein/messages/hide-reaction-menu-when-selecting"_q,
		{ u"selection"_q, u"reaction panel"_q },
	},
	{
		&kDisablePremiumStickerEffects,
		tr::lng_serein_disable_premium_sticker_effects,
		u"serein/messages/disable-premium-sticker-effects"_q,
		{ u"Premium"_q, u"sticker effects"_q },
	},
	{
		&kDisableEmojiInteractions,
		tr::lng_serein_disable_emoji_interactions,
		u"serein/messages/disable-emoji-interactions"_q,
		{ u"emoji"_q, u"interactions"_q },
	},
	{
		&kDisableMessageEffects,
		tr::lng_serein_disable_message_effects,
		u"serein/messages/disable-message-effects"_q,
		{ u"message effects"_q },
	},
	{
		&kRevealSpoilers,
		tr::lng_serein_reveal_spoilers,
		u"serein/messages/reveal-spoilers"_q,
		{ u"reveal"_q, u"spoilers"_q },
	},
	{
		&kHideQuickShare,
		tr::lng_serein_hide_quick_share,
		u"serein/messages/hide-quick-share"_q,
		{ u"quick forward"_q, u"share"_q },
	},
	{
		&kHideRecommendedChannels,
		tr::lng_serein_hide_recommended_channels,
		u"serein/messages/hide-recommended-channels"_q,
		{ u"recommended"_q, u"channels"_q },
	},
	{
		&kHidePremiumBadges,
		tr::lng_serein_hide_premium_badges,
		u"serein/messages/hide-premium-badges"_q,
		{ u"Premium"_q, u"emoji status"_q },
	},
	{
		&kHideSavedTags,
		tr::lng_serein_hide_saved_tags,
		u"serein/messages/hide-saved-tags"_q,
		{ u"saved messages"_q, u"tags"_q },
	},
	{
		&kHidePrivateChatActivities,
		tr::lng_serein_hide_private_chat_activities,
		u"serein/messages/hide-private-chat-activities"_q,
		{ u"typing"_q, u"recording"_q, u"private chat"_q },
	},
	{
		&kReadingSpacing,
		tr::lng_serein_reading_spacing,
		u"serein/messages/reading-spacing"_q,
		{ u"reading"_q, u"spacing"_q },
	},
	{
		&kRaiseSelectionLimit,
		tr::lng_serein_raise_selection_limit,
		u"serein/messages/raise-selection-limit"_q,
		{ u"select"_q, u"selection"_q, u"limit"_q, u"forward"_q, u"delete"_q, u"batch"_q },
	},
	{
		&kDoubleClickEditsOwn,
		tr::lng_serein_double_click_edits_own,
		u"serein/messages/double-click-edits-own"_q,
		{ u"double click"_q, u"edit"_q, u"own messages"_q },
	},
	{
		&kRevokePrivateChatDeletion,
		tr::lng_serein_revoke_private_chat_deletion,
		u"serein/messages/revoke-private-chat-deletion"_q,
		{ u"delete for everyone"_q, u"clear history"_q, u"private chat"_q },
	},
	{
		&kModerateReportSpam,
		tr::lng_serein_moderate_report_spam,
		u"serein/messages/moderate-report-spam"_q,
		{ u"report spam"_q, u"moderate"_q },
	},
	{
		&kModerateDeleteAll,
		tr::lng_serein_moderate_delete_all,
		u"serein/messages/moderate-delete-all"_q,
		{ u"delete all"_q, u"moderate"_q },
	},
	{
		&kModerateBan,
		tr::lng_serein_moderate_ban,
		u"serein/messages/moderate-ban"_q,
		{ u"ban"_q, u"moderate"_q },
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
	});
	AddText(builder, {
		.option = &kDeletedMark,
		.title = tr::lng_serein_deleted_mark_title,
		.id = u"serein/messages/deleted-mark"_q,
		.keywords = { u"deleted"_q, u"label"_q, u"text"_q, u"anti-recall"_q },
		.placeholder = tr::lng_serein_deleted_mark,
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

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	builder.addSectionButton({
		.title = (*kSubpageTitle)(),
		.targetSection = section,
		.icon = { kSubpageIcon },
		.keywords = { u"messages"_q, u"time"_q },
	});
}

} // namespace Serein::Messages
