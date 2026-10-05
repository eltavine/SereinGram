// Generated from proto/serein/settings/v1/interface.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/interface.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/interface.h"

namespace Serein::Hooks::Interface {

int BubbleRoundness() {
	return ForDevice().Get(Serein::Interface::kBubbleRoundness);
}

rpl::producer<int> BubbleRoundnessValue() {
	return ForDevice().Value(Serein::Interface::kBubbleRoundness);
}

int AvatarRoundness() {
	return ForDevice().Get(Serein::Interface::kAvatarRoundness);
}

rpl::producer<int> AvatarRoundnessValue() {
	return ForDevice().Value(Serein::Interface::kAvatarRoundness);
}

bool UniformAvatarShapes() {
	return ForDevice().Get(Serein::Interface::kUniformAvatarShapes);
}

rpl::producer<bool> UniformAvatarShapesValue() {
	return ForDevice().Value(Serein::Interface::kUniformAvatarShapes);
}

int TextMessageWidth() {
	return ForDevice().Get(Serein::Interface::kTextMessageWidth);
}

rpl::producer<int> TextMessageWidthValue() {
	return ForDevice().Value(Serein::Interface::kTextMessageWidth);
}

bool WideChannelPosts() {
	return ForDevice().Get(Serein::Interface::kWideChannelPosts);
}

rpl::producer<bool> WideChannelPostsValue() {
	return ForDevice().Value(Serein::Interface::kWideChannelPosts);
}

bool HideBubbleTail() {
	return ForDevice().Get(Serein::Interface::kHideBubbleTail);
}

rpl::producer<bool> HideBubbleTailValue() {
	return ForDevice().Value(Serein::Interface::kHideBubbleTail);
}

bool ThemeReplyColors() {
	return ForDevice().Get(Serein::Interface::kThemeReplyColors);
}

rpl::producer<bool> ThemeReplyColorsValue() {
	return ForDevice().Value(Serein::Interface::kThemeReplyColors);
}

bool HideReplyThumbnail() {
	return ForDevice().Get(Serein::Interface::kHideReplyThumbnail);
}

rpl::producer<bool> HideReplyThumbnailValue() {
	return ForDevice().Value(Serein::Interface::kHideReplyThumbnail);
}

bool IgnoreChatTheme() {
	return ForDevice().Get(Serein::Interface::kIgnoreChatTheme);
}

rpl::producer<bool> IgnoreChatThemeValue() {
	return ForDevice().Value(Serein::Interface::kIgnoreChatTheme);
}

QByteArray MainMenuConfig() {
	return ForDevice().Get(Serein::Interface::kMainMenuConfig);
}

rpl::producer<QByteArray> MainMenuConfigValue() {
	return ForDevice().Value(Serein::Interface::kMainMenuConfig);
}

bool MenuShortcuts() {
	return ForDevice().Get(Serein::Interface::kMenuShortcuts);
}

rpl::producer<bool> MenuShortcutsValue() {
	return ForDevice().Value(Serein::Interface::kMenuShortcuts);
}

int NotificationDelay() {
	return ForDevice().Get(Serein::Interface::kNotificationDelay);
}

rpl::producer<int> NotificationDelayValue() {
	return ForDevice().Value(Serein::Interface::kNotificationDelay);
}

int OtherDeviceNotificationDelay() {
	return ForDevice().Get(Serein::Interface::kOtherDeviceNotificationDelay);
}

rpl::producer<int> OtherDeviceNotificationDelayValue() {
	return ForDevice().Value(Serein::Interface::kOtherDeviceNotificationDelay);
}

bool CenterTopNotifications() {
	return ForDevice().Get(Serein::Interface::kCenterTopNotifications);
}

rpl::producer<bool> CenterTopNotificationsValue() {
	return ForDevice().Value(Serein::Interface::kCenterTopNotifications);
}

QByteArray QuietHours() {
	return ForDevice().Get(Serein::Interface::kQuietHours);
}

rpl::producer<QByteArray> QuietHoursValue() {
	return ForDevice().Value(Serein::Interface::kQuietHours);
}

bool HideAppIconBadge() {
	return ForDevice().Get(Serein::Interface::kHideAppIconBadge);
}

rpl::producer<bool> HideAppIconBadgeValue() {
	return ForDevice().Value(Serein::Interface::kHideAppIconBadge);
}

QByteArray GlobalShortcut() {
	return ForDevice().Get(Serein::Interface::kGlobalShortcut);
}

rpl::producer<QByteArray> GlobalShortcutValue() {
	return ForDevice().Value(Serein::Interface::kGlobalShortcut);
}

bool HalfwidthUiPunctuation() {
	return ForDevice().Get(Serein::Interface::kHalfwidthUiPunctuation);
}

rpl::producer<bool> HalfwidthUiPunctuationValue() {
	return ForDevice().Value(Serein::Interface::kHalfwidthUiPunctuation);
}

bool MoreAccounts() {
	return ForDevice().Get(Serein::Interface::kMoreAccounts);
}

rpl::producer<bool> MoreAccountsValue() {
	return ForDevice().Value(Serein::Interface::kMoreAccounts);
}

bool CheckUpdates() {
	return ForDevice().Get(Serein::Interface::kCheckUpdates);
}

rpl::producer<bool> CheckUpdatesValue() {
	return ForDevice().Value(Serein::Interface::kCheckUpdates);
}

bool OnlineStatusInChats() {
	return ForDevice().Get(Serein::Interface::kOnlineStatusInChats);
}

rpl::producer<bool> OnlineStatusInChatsValue() {
	return ForDevice().Value(Serein::Interface::kOnlineStatusInChats);
}

bool OnlineStatusInHeader() {
	return ForDevice().Get(Serein::Interface::kOnlineStatusInHeader);
}

rpl::producer<bool> OnlineStatusInHeaderValue() {
	return ForDevice().Value(Serein::Interface::kOnlineStatusInHeader);
}

bool OnlineStatusOnSenders() {
	return ForDevice().Get(Serein::Interface::kOnlineStatusOnSenders);
}

rpl::producer<bool> OnlineStatusOnSendersValue() {
	return ForDevice().Value(Serein::Interface::kOnlineStatusOnSenders);
}

bool OnlineStatusSelf() {
	return ForDevice().Get(Serein::Interface::kOnlineStatusSelf);
}

rpl::producer<bool> OnlineStatusSelfValue() {
	return ForDevice().Value(Serein::Interface::kOnlineStatusSelf);
}

bool OnlineStatusDetailed() {
	return ForDevice().Get(Serein::Interface::kOnlineStatusDetailed);
}

rpl::producer<bool> OnlineStatusDetailedValue() {
	return ForDevice().Value(Serein::Interface::kOnlineStatusDetailed);
}

bool PresetsOffered() {
	return ForDevice().Get(Serein::Interface::kPresetsOffered);
}

rpl::producer<bool> PresetsOfferedValue() {
	return ForDevice().Value(Serein::Interface::kPresetsOffered);
}

} // namespace Serein::Hooks::Interface
