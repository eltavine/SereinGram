#pragma once

#include "nagram/core/options.h"

namespace Nagram::Messages {

inline constexpr auto kRefreshMessageView = static_cast<unsigned>(
	Flag::RefreshMessageView);

inline constexpr auto kSecondsInMessages = Option<bool>{
	"nagram.secondsInMessages", Scope::Device, false,
	Category::Messages, "lng_nagram_seconds_in_messages", kRefreshMessageView };
inline constexpr auto kShowForwardedMessageDate = Option<bool>{
	"nagram.showForwardedMessageDate", Scope::Device, false,
	Category::Messages, "lng_nagram_show_forwarded_message_date", kRefreshMessageView };
inline constexpr auto kShowServiceTime = Option<bool>{
	"nagram.showServiceTime", Scope::Device, false,
	Category::Messages, "lng_nagram_show_service_time", kRefreshMessageView };
inline constexpr auto kShowMessageId = Option<bool>{
	"nagram.showMessageId", Scope::Device, false,
	Category::Messages, "lng_nagram_show_message_id", kRefreshMessageView };
inline constexpr auto kExactMessageCounters = Option<bool>{
	"nagram.exactMessageCounters", Scope::Device, false,
	Category::Messages, "lng_nagram_exact_message_counters", kRefreshMessageView };
inline constexpr auto kHideMessageViews = Option<bool>{
	"nagram.hideMessageViews", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_message_views", kRefreshMessageView };
inline constexpr auto kHideChannelSignature = Option<bool>{
	"nagram.hideChannelSignature", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_channel_signature", kRefreshMessageView };
inline constexpr auto kHideEditedBadge = Option<bool>{
	"nagram.hideEditedBadge", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_edited_badge", kRefreshMessageView };
inline const auto kEditedMark = Option<QString>{
	"nagram.editedMark", Scope::Device, QString(),
	Category::Messages, "lng_nagram_edited_mark", kRefreshMessageView };
inline constexpr auto kHideReactions = Option<bool>{
	"nagram.hideReactions", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_reactions", kRefreshMessageView };
inline constexpr auto kHidePrivateReactions = Option<bool>{
	"nagram.hidePrivateReactions", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_private_reactions", kRefreshMessageView };
inline constexpr auto kHideGroupReactions = Option<bool>{
	"nagram.hideGroupReactions", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_group_reactions", kRefreshMessageView };
inline constexpr auto kHideChannelReactions = Option<bool>{
	"nagram.hideChannelReactions", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_channel_reactions", kRefreshMessageView };
inline constexpr auto kHideReactionMenu = Option<bool>{
	"nagram.hideReactionMenu", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_reaction_menu" };
inline constexpr auto kHideReactionMenuWhenSelecting = Option<bool>{
	"nagram.hideReactionMenuWhenSelecting", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_reaction_menu_when_selecting" };
inline constexpr auto kDisablePremiumStickerEffects = Option<bool>{
	"nagram.disablePremiumStickerEffects", Scope::Device, false,
	Category::Messages, "lng_nagram_disable_premium_sticker_effects" };
inline constexpr auto kDisableEmojiInteractions = Option<bool>{
	"nagram.disableEmojiInteractions", Scope::Device, false,
	Category::Messages, "lng_nagram_disable_emoji_interactions" };
inline constexpr auto kDisableMessageEffects = Option<bool>{
	"nagram.disableMessageEffects", Scope::Device, false,
	Category::Messages, "lng_nagram_disable_message_effects" };

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
}

} // namespace Nagram::Messages
