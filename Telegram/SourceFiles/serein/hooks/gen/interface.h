// Generated from proto/serein/settings/v1/interface.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <rpl/producer.h>

namespace Serein::Hooks::Interface {

[[nodiscard]] int BubbleRoundness();
[[nodiscard]] rpl::producer<int> BubbleRoundnessValue();
[[nodiscard]] int AvatarRoundness();
[[nodiscard]] rpl::producer<int> AvatarRoundnessValue();
[[nodiscard]] bool UniformAvatarShapes();
[[nodiscard]] rpl::producer<bool> UniformAvatarShapesValue();
[[nodiscard]] int TextMessageWidth();
[[nodiscard]] rpl::producer<int> TextMessageWidthValue();
[[nodiscard]] bool WideChannelPosts();
[[nodiscard]] rpl::producer<bool> WideChannelPostsValue();
[[nodiscard]] bool HideBubbleTail();
[[nodiscard]] rpl::producer<bool> HideBubbleTailValue();
[[nodiscard]] bool ThemeReplyColors();
[[nodiscard]] rpl::producer<bool> ThemeReplyColorsValue();
[[nodiscard]] bool HideReplyThumbnail();
[[nodiscard]] rpl::producer<bool> HideReplyThumbnailValue();
[[nodiscard]] bool IgnoreChatTheme();
[[nodiscard]] rpl::producer<bool> IgnoreChatThemeValue();
[[nodiscard]] QByteArray MainMenuConfig();
[[nodiscard]] rpl::producer<QByteArray> MainMenuConfigValue();
[[nodiscard]] bool HideAppIconBadge();
[[nodiscard]] rpl::producer<bool> HideAppIconBadgeValue();
[[nodiscard]] int NotificationDelay();
[[nodiscard]] rpl::producer<int> NotificationDelayValue();
[[nodiscard]] int OtherDeviceNotificationDelay();
[[nodiscard]] rpl::producer<int> OtherDeviceNotificationDelayValue();
[[nodiscard]] bool CenterTopNotifications();
[[nodiscard]] rpl::producer<bool> CenterTopNotificationsValue();
[[nodiscard]] bool MenuShortcuts();
[[nodiscard]] rpl::producer<bool> MenuShortcutsValue();
[[nodiscard]] QByteArray GlobalShortcut();
[[nodiscard]] rpl::producer<QByteArray> GlobalShortcutValue();
[[nodiscard]] bool HalfwidthUiPunctuation();
[[nodiscard]] rpl::producer<bool> HalfwidthUiPunctuationValue();
[[nodiscard]] bool MoreAccounts();
[[nodiscard]] rpl::producer<bool> MoreAccountsValue();
[[nodiscard]] bool CheckUpdates();
[[nodiscard]] rpl::producer<bool> CheckUpdatesValue();
[[nodiscard]] bool PresetsOffered();
[[nodiscard]] rpl::producer<bool> PresetsOfferedValue();

} // namespace Serein::Hooks::Interface
