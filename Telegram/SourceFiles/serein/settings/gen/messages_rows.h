// Generated from proto/serein/settings/v1/messages.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/messages.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Messages {

inline const auto kToggleRows = std::array<ToggleRow, 24>{ {
	{
		&kSecondsInMessages,
		tr::lng_serein_seconds_in_messages,
		u"serein/messages/seconds-in-messages"_q,
		{  },
	},
	{
		&kShowForwardedMessageDate,
		tr::lng_serein_show_forwarded_message_date,
		u"serein/messages/show-forwarded-message-date"_q,
		{  },
	},
	{
		&kShowServiceTime,
		tr::lng_serein_show_service_time,
		u"serein/messages/show-service-time"_q,
		{  },
	},
	{
		&kShowMessageId,
		tr::lng_serein_show_message_id,
		u"serein/messages/show-message-id"_q,
		{  },
	},
	{
		&kExactMessageCounters,
		tr::lng_serein_exact_message_counters,
		u"serein/messages/exact-message-counters"_q,
		{  },
	},
	{
		&kHideMessageViews,
		tr::lng_serein_hide_message_views,
		u"serein/messages/hide-message-views"_q,
		{  },
	},
	{
		&kHideChannelSignature,
		tr::lng_serein_hide_channel_signature,
		u"serein/messages/hide-channel-signature"_q,
		{  },
	},
	{
		&kHideEditedBadge,
		tr::lng_serein_hide_edited_badge,
		u"serein/messages/hide-edited-badge"_q,
		{  },
	},
	{
		&kHideReactions,
		tr::lng_serein_hide_reactions,
		u"serein/messages/hide-reactions"_q,
		{  },
	},
	{
		&kHidePrivateReactions,
		tr::lng_serein_hide_private_reactions,
		u"serein/messages/hide-private-reactions"_q,
		{  },
	},
	{
		&kHideGroupReactions,
		tr::lng_serein_hide_group_reactions,
		u"serein/messages/hide-group-reactions"_q,
		{  },
	},
	{
		&kHideChannelReactions,
		tr::lng_serein_hide_channel_reactions,
		u"serein/messages/hide-channel-reactions"_q,
		{  },
	},
	{
		&kHideReactionMenu,
		tr::lng_serein_hide_reaction_menu,
		u"serein/messages/hide-reaction-menu"_q,
		{  },
	},
	{
		&kHideReactionMenuWhenSelecting,
		tr::lng_serein_hide_reaction_menu_when_selecting,
		u"serein/messages/hide-reaction-menu-when-selecting"_q,
		{  },
	},
	{
		&kDisablePremiumStickerEffects,
		tr::lng_serein_disable_premium_sticker_effects,
		u"serein/messages/disable-premium-sticker-effects"_q,
		{  },
	},
	{
		&kDisableEmojiInteractions,
		tr::lng_serein_disable_emoji_interactions,
		u"serein/messages/disable-emoji-interactions"_q,
		{  },
	},
	{
		&kDisableMessageEffects,
		tr::lng_serein_disable_message_effects,
		u"serein/messages/disable-message-effects"_q,
		{  },
	},
	{
		&kRevealSpoilers,
		tr::lng_serein_reveal_spoilers,
		u"serein/messages/reveal-spoilers"_q,
		{  },
	},
	{
		&kHideQuickShare,
		tr::lng_serein_hide_quick_share,
		u"serein/messages/hide-quick-share"_q,
		{  },
	},
	{
		&kHideRecommendedChannels,
		tr::lng_serein_hide_recommended_channels,
		u"serein/messages/hide-recommended-channels"_q,
		{  },
	},
	{
		&kHidePremiumBadges,
		tr::lng_serein_hide_premium_badges,
		u"serein/messages/hide-premium-badges"_q,
		{  },
	},
	{
		&kHideSavedTags,
		tr::lng_serein_hide_saved_tags,
		u"serein/messages/hide-saved-tags"_q,
		{  },
	},
	{
		&kHidePrivateChatActivities,
		tr::lng_serein_hide_private_chat_activities,
		u"serein/messages/hide-private-chat-activities"_q,
		{  },
	},
	{
		&kReadingSpacing,
		tr::lng_serein_reading_spacing,
		u"serein/messages/reading-spacing"_q,
		{  },
	},
} };

} // namespace Serein::Messages
