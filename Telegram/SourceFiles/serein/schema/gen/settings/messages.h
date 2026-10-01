// Generated from proto/serein/settings/v1/messages.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Messages {

inline constexpr auto kSecondsInMessages = Option<bool>{
	"serein.secondsInMessages",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_seconds_in_messages",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kShowForwardedMessageDate = Option<bool>{
	"serein.showForwardedMessageDate",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_show_forwarded_message_date",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kShowServiceTime = Option<bool>{
	"serein.showServiceTime",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_show_service_time",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kShowMessageId = Option<bool>{
	"serein.showMessageId",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_show_message_id",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kExactMessageCounters = Option<bool>{
	"serein.exactMessageCounters",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_exact_message_counters",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideMessageViews = Option<bool>{
	"serein.hideMessageViews",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_message_views",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideChannelSignature = Option<bool>{
	"serein.hideChannelSignature",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_channel_signature",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideEditedBadge = Option<bool>{
	"serein.hideEditedBadge",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_edited_badge",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline const auto kEditedMark = Option<QString>{
	"serein.editedMark",
	Scope::Device,
	QString(),
	Category::Messages,
	"lng_serein_edited_mark",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 64));
	} };
inline const auto kDeletedMark = Option<QString>{
	"serein.deletedMark",
	Scope::Device,
	QString(),
	Category::Messages,
	"lng_serein_deleted_mark_title",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 64));
	} };
inline constexpr auto kFadeDeletedMessages = Option<bool>{
	"serein.fadeDeletedMessages",
	Scope::Device,
	true,
	Category::Messages,
	"lng_serein_fade_deleted_messages",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kShowChannelBadge = Option<bool>{
	"serein.showChannelBadge",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_show_channel_badge",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideReactions = Option<bool>{
	"serein.hideReactions",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_reactions",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHidePrivateReactions = Option<bool>{
	"serein.hidePrivateReactions",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_private_reactions",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideGroupReactions = Option<bool>{
	"serein.hideGroupReactions",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_group_reactions",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideChannelReactions = Option<bool>{
	"serein.hideChannelReactions",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_channel_reactions",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideReactionMenu = Option<bool>{
	"serein.hideReactionMenu",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_reaction_menu",
	0 };
inline constexpr auto kHideReactionMenuWhenSelecting = Option<bool>{
	"serein.hideReactionMenuWhenSelecting",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_reaction_menu_when_selecting",
	0 };
inline constexpr auto kDisablePremiumStickerEffects = Option<bool>{
	"serein.disablePremiumStickerEffects",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_disable_premium_sticker_effects",
	0 };
inline constexpr auto kDisableEmojiInteractions = Option<bool>{
	"serein.disableEmojiInteractions",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_disable_emoji_interactions",
	0 };
inline constexpr auto kDisableMessageEffects = Option<bool>{
	"serein.disableMessageEffects",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_disable_message_effects",
	0 };
inline constexpr auto kRevealSpoilers = Option<bool>{
	"serein.revealSpoilers",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_reveal_spoilers",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideQuickShare = Option<bool>{
	"serein.hideQuickShare",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_quick_share",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideRecommendedChannels = Option<bool>{
	"serein.hideRecommendedChannels",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_recommended_channels",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHidePremiumBadges = Option<bool>{
	"serein.hidePremiumBadges",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_premium_badges",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kHideSavedTags = Option<bool>{
	"serein.hideSavedTags",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_saved_tags",
	0 };
inline constexpr auto kHidePrivateChatActivities = Option<bool>{
	"serein.hidePrivateChatActivities",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_hide_private_chat_activities",
	0 };
inline constexpr auto kReadingSpacing = Option<bool>{
	"serein.readingSpacing",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_reading_spacing",
	static_cast<unsigned>(Flag::RefreshMessageView) };
inline constexpr auto kReadingChinese = Option<int>{
	"serein.readingChinese",
	Scope::Device,
	0,
	Category::Messages,
	"lng_serein_reading_chinese",
	static_cast<unsigned>(Flag::RefreshMessageView),
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 2));
	} };
inline constexpr auto kRevokePrivateChatDeletion = Option<bool>{
	"serein.revokePrivateChatDeletion",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_revoke_private_chat_deletion",
	0 };
inline constexpr auto kModerateReportSpam = Option<bool>{
	"serein.moderateReportSpam",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_moderate_report_spam",
	0 };
inline constexpr auto kModerateDeleteAll = Option<bool>{
	"serein.moderateDeleteAll",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_moderate_delete_all",
	0 };
inline constexpr auto kModerateBan = Option<bool>{
	"serein.moderateBan",
	Scope::Device,
	false,
	Category::Messages,
	"lng_serein_moderate_ban",
	0 };

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
	Expects(registry.Add(kDeletedMark));
	Expects(registry.Add(kFadeDeletedMessages));
	Expects(registry.Add(kShowChannelBadge));
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
	Expects(registry.Add(kRevokePrivateChatDeletion));
	Expects(registry.Add(kModerateReportSpam));
	Expects(registry.Add(kModerateDeleteAll));
	Expects(registry.Add(kModerateBan));
}

} // namespace Serein::Messages
