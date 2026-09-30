// Generated from proto/serein/settings/v1/messages.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/messages.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/messages.h"

namespace Serein::Hooks::Messages {

bool SecondsInMessages() {
	return ForDevice().Get(Serein::Messages::kSecondsInMessages);
}

rpl::producer<bool> SecondsInMessagesValue() {
	return ForDevice().Value(Serein::Messages::kSecondsInMessages);
}

bool ShowForwardedMessageDate() {
	return ForDevice().Get(Serein::Messages::kShowForwardedMessageDate);
}

rpl::producer<bool> ShowForwardedMessageDateValue() {
	return ForDevice().Value(Serein::Messages::kShowForwardedMessageDate);
}

bool ShowServiceTime() {
	return ForDevice().Get(Serein::Messages::kShowServiceTime);
}

rpl::producer<bool> ShowServiceTimeValue() {
	return ForDevice().Value(Serein::Messages::kShowServiceTime);
}

bool ShowMessageId() {
	return ForDevice().Get(Serein::Messages::kShowMessageId);
}

rpl::producer<bool> ShowMessageIdValue() {
	return ForDevice().Value(Serein::Messages::kShowMessageId);
}

bool ExactMessageCounters() {
	return ForDevice().Get(Serein::Messages::kExactMessageCounters);
}

rpl::producer<bool> ExactMessageCountersValue() {
	return ForDevice().Value(Serein::Messages::kExactMessageCounters);
}

bool HideMessageViews() {
	return ForDevice().Get(Serein::Messages::kHideMessageViews);
}

rpl::producer<bool> HideMessageViewsValue() {
	return ForDevice().Value(Serein::Messages::kHideMessageViews);
}

bool HideChannelSignature() {
	return ForDevice().Get(Serein::Messages::kHideChannelSignature);
}

rpl::producer<bool> HideChannelSignatureValue() {
	return ForDevice().Value(Serein::Messages::kHideChannelSignature);
}

bool HideEditedBadge() {
	return ForDevice().Get(Serein::Messages::kHideEditedBadge);
}

rpl::producer<bool> HideEditedBadgeValue() {
	return ForDevice().Value(Serein::Messages::kHideEditedBadge);
}

QString EditedMark() {
	return ForDevice().Get(Serein::Messages::kEditedMark);
}

rpl::producer<QString> EditedMarkValue() {
	return ForDevice().Value(Serein::Messages::kEditedMark);
}

bool HideReactions() {
	return ForDevice().Get(Serein::Messages::kHideReactions);
}

rpl::producer<bool> HideReactionsValue() {
	return ForDevice().Value(Serein::Messages::kHideReactions);
}

bool HidePrivateReactions() {
	return ForDevice().Get(Serein::Messages::kHidePrivateReactions);
}

rpl::producer<bool> HidePrivateReactionsValue() {
	return ForDevice().Value(Serein::Messages::kHidePrivateReactions);
}

bool HideGroupReactions() {
	return ForDevice().Get(Serein::Messages::kHideGroupReactions);
}

rpl::producer<bool> HideGroupReactionsValue() {
	return ForDevice().Value(Serein::Messages::kHideGroupReactions);
}

bool HideChannelReactions() {
	return ForDevice().Get(Serein::Messages::kHideChannelReactions);
}

rpl::producer<bool> HideChannelReactionsValue() {
	return ForDevice().Value(Serein::Messages::kHideChannelReactions);
}

bool HideReactionMenu() {
	return ForDevice().Get(Serein::Messages::kHideReactionMenu);
}

rpl::producer<bool> HideReactionMenuValue() {
	return ForDevice().Value(Serein::Messages::kHideReactionMenu);
}

bool HideReactionMenuWhenSelecting() {
	return ForDevice().Get(Serein::Messages::kHideReactionMenuWhenSelecting);
}

rpl::producer<bool> HideReactionMenuWhenSelectingValue() {
	return ForDevice().Value(Serein::Messages::kHideReactionMenuWhenSelecting);
}

bool DisablePremiumStickerEffects() {
	return ForDevice().Get(Serein::Messages::kDisablePremiumStickerEffects);
}

rpl::producer<bool> DisablePremiumStickerEffectsValue() {
	return ForDevice().Value(Serein::Messages::kDisablePremiumStickerEffects);
}

bool DisableEmojiInteractions() {
	return ForDevice().Get(Serein::Messages::kDisableEmojiInteractions);
}

rpl::producer<bool> DisableEmojiInteractionsValue() {
	return ForDevice().Value(Serein::Messages::kDisableEmojiInteractions);
}

bool DisableMessageEffects() {
	return ForDevice().Get(Serein::Messages::kDisableMessageEffects);
}

rpl::producer<bool> DisableMessageEffectsValue() {
	return ForDevice().Value(Serein::Messages::kDisableMessageEffects);
}

bool RevealSpoilers() {
	return ForDevice().Get(Serein::Messages::kRevealSpoilers);
}

rpl::producer<bool> RevealSpoilersValue() {
	return ForDevice().Value(Serein::Messages::kRevealSpoilers);
}

bool HideQuickShare() {
	return ForDevice().Get(Serein::Messages::kHideQuickShare);
}

rpl::producer<bool> HideQuickShareValue() {
	return ForDevice().Value(Serein::Messages::kHideQuickShare);
}

bool HideRecommendedChannels() {
	return ForDevice().Get(Serein::Messages::kHideRecommendedChannels);
}

rpl::producer<bool> HideRecommendedChannelsValue() {
	return ForDevice().Value(Serein::Messages::kHideRecommendedChannels);
}

bool HidePremiumBadges() {
	return ForDevice().Get(Serein::Messages::kHidePremiumBadges);
}

rpl::producer<bool> HidePremiumBadgesValue() {
	return ForDevice().Value(Serein::Messages::kHidePremiumBadges);
}

bool HideSavedTags() {
	return ForDevice().Get(Serein::Messages::kHideSavedTags);
}

rpl::producer<bool> HideSavedTagsValue() {
	return ForDevice().Value(Serein::Messages::kHideSavedTags);
}

bool HidePrivateChatActivities() {
	return ForDevice().Get(Serein::Messages::kHidePrivateChatActivities);
}

rpl::producer<bool> HidePrivateChatActivitiesValue() {
	return ForDevice().Value(Serein::Messages::kHidePrivateChatActivities);
}

bool ReadingSpacing() {
	return ForDevice().Get(Serein::Messages::kReadingSpacing);
}

rpl::producer<bool> ReadingSpacingValue() {
	return ForDevice().Value(Serein::Messages::kReadingSpacing);
}

int ReadingChinese() {
	return ForDevice().Get(Serein::Messages::kReadingChinese);
}

rpl::producer<int> ReadingChineseValue() {
	return ForDevice().Value(Serein::Messages::kReadingChinese);
}

} // namespace Serein::Hooks::Messages
