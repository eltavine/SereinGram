// Generated from proto/serein/settings/v1/messages.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QString>
#include <rpl/producer.h>

namespace Serein::Hooks::Messages {

[[nodiscard]] bool SecondsInMessages();
[[nodiscard]] rpl::producer<bool> SecondsInMessagesValue();
[[nodiscard]] bool ShowForwardedMessageDate();
[[nodiscard]] rpl::producer<bool> ShowForwardedMessageDateValue();
[[nodiscard]] bool ShowServiceTime();
[[nodiscard]] rpl::producer<bool> ShowServiceTimeValue();
[[nodiscard]] bool ShowMessageId();
[[nodiscard]] rpl::producer<bool> ShowMessageIdValue();
[[nodiscard]] bool PersianCalendar();
[[nodiscard]] rpl::producer<bool> PersianCalendarValue();
[[nodiscard]] bool RaiseSelectionLimit();
[[nodiscard]] rpl::producer<bool> RaiseSelectionLimitValue();
[[nodiscard]] bool ExactMessageCounters();
[[nodiscard]] rpl::producer<bool> ExactMessageCountersValue();
[[nodiscard]] bool HideMessageViews();
[[nodiscard]] rpl::producer<bool> HideMessageViewsValue();
[[nodiscard]] bool HideChannelSignature();
[[nodiscard]] rpl::producer<bool> HideChannelSignatureValue();
[[nodiscard]] bool HideEditedBadge();
[[nodiscard]] rpl::producer<bool> HideEditedBadgeValue();
[[nodiscard]] QString EditedMark();
[[nodiscard]] rpl::producer<QString> EditedMarkValue();
[[nodiscard]] QString DeletedMark();
[[nodiscard]] rpl::producer<QString> DeletedMarkValue();
[[nodiscard]] bool FadeDeletedMessages();
[[nodiscard]] rpl::producer<bool> FadeDeletedMessagesValue();
[[nodiscard]] bool ShowChannelBadge();
[[nodiscard]] rpl::producer<bool> ShowChannelBadgeValue();
[[nodiscard]] bool HideReactions();
[[nodiscard]] rpl::producer<bool> HideReactionsValue();
[[nodiscard]] bool HidePrivateReactions();
[[nodiscard]] rpl::producer<bool> HidePrivateReactionsValue();
[[nodiscard]] bool HideGroupReactions();
[[nodiscard]] rpl::producer<bool> HideGroupReactionsValue();
[[nodiscard]] bool HideChannelReactions();
[[nodiscard]] rpl::producer<bool> HideChannelReactionsValue();
[[nodiscard]] bool HideReactionMenu();
[[nodiscard]] rpl::producer<bool> HideReactionMenuValue();
[[nodiscard]] bool HideReactionMenuWhenSelecting();
[[nodiscard]] rpl::producer<bool> HideReactionMenuWhenSelectingValue();
[[nodiscard]] bool DisablePremiumStickerEffects();
[[nodiscard]] rpl::producer<bool> DisablePremiumStickerEffectsValue();
[[nodiscard]] bool DisableEmojiInteractions();
[[nodiscard]] rpl::producer<bool> DisableEmojiInteractionsValue();
[[nodiscard]] bool DisableMessageEffects();
[[nodiscard]] rpl::producer<bool> DisableMessageEffectsValue();
[[nodiscard]] bool RevealSpoilers();
[[nodiscard]] rpl::producer<bool> RevealSpoilersValue();
[[nodiscard]] bool HideQuickShare();
[[nodiscard]] rpl::producer<bool> HideQuickShareValue();
[[nodiscard]] bool HideRecommendedChannels();
[[nodiscard]] rpl::producer<bool> HideRecommendedChannelsValue();
[[nodiscard]] bool HidePremiumBadges();
[[nodiscard]] rpl::producer<bool> HidePremiumBadgesValue();
[[nodiscard]] bool HideSavedTags();
[[nodiscard]] rpl::producer<bool> HideSavedTagsValue();
[[nodiscard]] bool HidePrivateChatActivities();
[[nodiscard]] rpl::producer<bool> HidePrivateChatActivitiesValue();
[[nodiscard]] bool ReadingSpacing();
[[nodiscard]] rpl::producer<bool> ReadingSpacingValue();
[[nodiscard]] int ReadingChinese();
[[nodiscard]] rpl::producer<int> ReadingChineseValue();
[[nodiscard]] bool RevokePrivateChatDeletion();
[[nodiscard]] rpl::producer<bool> RevokePrivateChatDeletionValue();
[[nodiscard]] bool ModerateReportSpam();
[[nodiscard]] rpl::producer<bool> ModerateReportSpamValue();
[[nodiscard]] bool ModerateDeleteAll();
[[nodiscard]] rpl::producer<bool> ModerateDeleteAllValue();
[[nodiscard]] bool ModerateBan();
[[nodiscard]] rpl::producer<bool> ModerateBanValue();

} // namespace Serein::Hooks::Messages
