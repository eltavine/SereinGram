#pragma once

#include "serein/core/options.h"

namespace Serein::Messages {

inline constexpr auto kRefreshMessageView = static_cast<unsigned>(
	Flag::RefreshMessageView);

inline constexpr auto kSecondsInMessages = Option<bool>{
	"serein.secondsInMessages", Scope::Device, false,
	Category::Messages, "lng_serein_seconds_in_messages", kRefreshMessageView };
inline constexpr auto kShowForwardedMessageDate = Option<bool>{
	"serein.showForwardedMessageDate", Scope::Device, false,
	Category::Messages, "lng_serein_show_forwarded_message_date", kRefreshMessageView };
inline constexpr auto kShowServiceTime = Option<bool>{
	"serein.showServiceTime", Scope::Device, false,
	Category::Messages, "lng_serein_show_service_time", kRefreshMessageView };
inline constexpr auto kShowMessageId = Option<bool>{
	"serein.showMessageId", Scope::Device, false,
	Category::Messages, "lng_serein_show_message_id", kRefreshMessageView };
inline constexpr auto kExactMessageCounters = Option<bool>{
	"serein.exactMessageCounters", Scope::Device, false,
	Category::Messages, "lng_serein_exact_message_counters", kRefreshMessageView };
inline constexpr auto kHideMessageViews = Option<bool>{
	"serein.hideMessageViews", Scope::Device, false,
	Category::Messages, "lng_serein_hide_message_views", kRefreshMessageView };
inline constexpr auto kHideChannelSignature = Option<bool>{
	"serein.hideChannelSignature", Scope::Device, false,
	Category::Messages, "lng_serein_hide_channel_signature", kRefreshMessageView };
inline constexpr auto kHideEditedBadge = Option<bool>{
	"serein.hideEditedBadge", Scope::Device, false,
	Category::Messages, "lng_serein_hide_edited_badge", kRefreshMessageView };
inline const auto kEditedMark = Option<QString>{
	"serein.editedMark", Scope::Device, QString(),
	Category::Messages, "lng_serein_edited_mark", kRefreshMessageView };
inline constexpr auto kHideReactions = Option<bool>{
	"serein.hideReactions", Scope::Device, false,
	Category::Messages, "lng_serein_hide_reactions", kRefreshMessageView };
inline constexpr auto kHidePrivateReactions = Option<bool>{
	"serein.hidePrivateReactions", Scope::Device, false,
	Category::Messages, "lng_serein_hide_private_reactions", kRefreshMessageView };
inline constexpr auto kHideGroupReactions = Option<bool>{
	"serein.hideGroupReactions", Scope::Device, false,
	Category::Messages, "lng_serein_hide_group_reactions", kRefreshMessageView };
inline constexpr auto kHideChannelReactions = Option<bool>{
	"serein.hideChannelReactions", Scope::Device, false,
	Category::Messages, "lng_serein_hide_channel_reactions", kRefreshMessageView };
inline constexpr auto kHideReactionMenu = Option<bool>{
	"serein.hideReactionMenu", Scope::Device, false,
	Category::Messages, "lng_serein_hide_reaction_menu" };
inline constexpr auto kHideReactionMenuWhenSelecting = Option<bool>{
	"serein.hideReactionMenuWhenSelecting", Scope::Device, false,
	Category::Messages, "lng_serein_hide_reaction_menu_when_selecting" };
inline constexpr auto kDisablePremiumStickerEffects = Option<bool>{
	"serein.disablePremiumStickerEffects", Scope::Device, false,
	Category::Messages, "lng_serein_disable_premium_sticker_effects" };
inline constexpr auto kDisableEmojiInteractions = Option<bool>{
	"serein.disableEmojiInteractions", Scope::Device, false,
	Category::Messages, "lng_serein_disable_emoji_interactions" };
inline constexpr auto kDisableMessageEffects = Option<bool>{
	"serein.disableMessageEffects", Scope::Device, false,
	Category::Messages, "lng_serein_disable_message_effects" };
inline constexpr auto kRevealSpoilers = Option<bool>{
	"serein.revealSpoilers", Scope::Device, false,
	Category::Messages, "lng_serein_reveal_spoilers", kRefreshMessageView };
inline constexpr auto kHideQuickShare = Option<bool>{
	"serein.hideQuickShare", Scope::Device, false,
	Category::Messages, "lng_serein_hide_quick_share", kRefreshMessageView };
inline constexpr auto kHideRecommendedChannels = Option<bool>{
	"serein.hideRecommendedChannels", Scope::Device, false,
	Category::Messages, "lng_serein_hide_recommended_channels", kRefreshMessageView };
inline constexpr auto kHidePremiumBadges = Option<bool>{
	"serein.hidePremiumBadges", Scope::Device, false,
	Category::Messages, "lng_serein_hide_premium_badges", kRefreshMessageView };
inline constexpr auto kHideSavedTags = Option<bool>{
	"serein.hideSavedTags", Scope::Device, false,
	Category::Messages, "lng_serein_hide_saved_tags" };
inline constexpr auto kHidePrivateChatActivities = Option<bool>{
	"serein.hidePrivateChatActivities", Scope::Device, false,
	Category::Messages, "lng_serein_hide_private_chat_activities" };
inline constexpr auto kReadingSpacing = Option<bool>{
	"serein.readingSpacing", Scope::Device, false,
	Category::Messages, "lng_serein_reading_spacing", kRefreshMessageView };
inline constexpr auto kReadingChinese = Option<int>{
	"serein.readingChinese", Scope::Device, 0,
	Category::Messages, "lng_serein_reading_chinese", kRefreshMessageView,
	[](const int &value) { return value >= 0 && value <= 2; } };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kSecondsInMessages));
	Expects(registry.Add(kShowForwardedMessageDate));
	Expects(registry.Add(kShowServiceTime));
	Expects(registry.Add(kShowMessageId));
	Expects(registry.Add(kExactMessageCounters));
	Expects(registry.Add(kHideMessageViews));
	Expects(registry.Add(kHideChannelSignature));
	Expects(registry.Add(kHideEditedBadge));
	Expects(registry.Add(kEditedMark));
	Expects(registry.Add(kHideReactions));
	Expects(registry.Add(kHidePrivateReactions));
	Expects(registry.Add(kHideGroupReactions));
	Expects(registry.Add(kHideChannelReactions));
	Expects(registry.Add(kHideReactionMenu));
	Expects(registry.Add(kHideReactionMenuWhenSelecting));
	Expects(registry.Add(kDisablePremiumStickerEffects));
	Expects(registry.Add(kDisableEmojiInteractions));
	Expects(registry.Add(kDisableMessageEffects));
	Expects(registry.Add(kRevealSpoilers));
	Expects(registry.Add(kHideQuickShare));
	Expects(registry.Add(kHideRecommendedChannels));
	Expects(registry.Add(kHidePremiumBadges));
	Expects(registry.Add(kHideSavedTags));
	Expects(registry.Add(kHidePrivateChatActivities));
	Expects(registry.Add(kReadingSpacing));
	Expects(registry.Add(kReadingChinese));
}

} // namespace Serein::Messages
