// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QString>
#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::Privacy {

[[nodiscard]] bool DemoMode();
[[nodiscard]] rpl::producer<bool> DemoModeValue();
[[nodiscard]] bool AutoDemoMode();
[[nodiscard]] rpl::producer<bool> AutoDemoModeValue();
[[nodiscard]] bool LockSettings();
[[nodiscard]] rpl::producer<bool> LockSettingsValue();
[[nodiscard]] int ProfileIdFormat();
[[nodiscard]] rpl::producer<int> ProfileIdFormatValue();
[[nodiscard]] bool ShowProfileDc();
[[nodiscard]] rpl::producer<bool> ShowProfileDcValue();
[[nodiscard]] bool ShowRegistrationDate();
[[nodiscard]] rpl::producer<bool> ShowRegistrationDateValue();
[[nodiscard]] bool ShowContactStatus();
[[nodiscard]] rpl::producer<bool> ShowContactStatusValue();
[[nodiscard]] bool ShowSessionDetails();
[[nodiscard]] rpl::producer<bool> ShowSessionDetailsValue();
[[nodiscard]] bool LocalNames();
[[nodiscard]] rpl::producer<bool> LocalNamesValue();
[[nodiscard]] bool HideProfileGifts();
[[nodiscard]] rpl::producer<bool> HideProfileGiftsValue();
[[nodiscard]] bool HideReadTime();
[[nodiscard]] rpl::producer<bool> HideReadTimeValue();
[[nodiscard]] bool HideSharePhonePrompt();
[[nodiscard]] rpl::producer<bool> HideSharePhonePromptValue();
[[nodiscard]] bool HideCreateTodo();
[[nodiscard]] rpl::producer<bool> HideCreateTodoValue();
[[nodiscard]] bool SaveProtectedContent();
[[nodiscard]] rpl::producer<bool> SaveProtectedContentValue();
[[nodiscard]] bool AutoBackupExports();
[[nodiscard]] rpl::producer<bool> AutoBackupExportsValue();
[[nodiscard]] bool HiddenChatsEnabled();
[[nodiscard]] rpl::producer<bool> HiddenChatsEnabledValue();
[[nodiscard]] QString HiddenChatIds(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QString> HiddenChatIdsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HiddenAccountsEnabled();
[[nodiscard]] rpl::producer<bool> HiddenAccountsEnabledValue();
[[nodiscard]] QString HiddenAccountIds();
[[nodiscard]] rpl::producer<QString> HiddenAccountIdsValue();
[[nodiscard]] bool AccountNetworkControls();
[[nodiscard]] rpl::producer<bool> AccountNetworkControlsValue();

} // namespace Serein::Hooks::Privacy
