// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include <rpl/producer.h>

namespace Serein::Hooks::Privacy {

[[nodiscard]] bool DemoMode();
[[nodiscard]] rpl::producer<bool> DemoModeValue();
[[nodiscard]] bool AutoDemoMode();
[[nodiscard]] rpl::producer<bool> AutoDemoModeValue();
[[nodiscard]] bool LockSettings();
[[nodiscard]] rpl::producer<bool> LockSettingsValue();
[[nodiscard]] bool HideReadTime();
[[nodiscard]] rpl::producer<bool> HideReadTimeValue();
[[nodiscard]] bool HideSharePhonePrompt();
[[nodiscard]] rpl::producer<bool> HideSharePhonePromptValue();
[[nodiscard]] int ProfileIdFormat();
[[nodiscard]] rpl::producer<int> ProfileIdFormatValue();
[[nodiscard]] bool ShowProfileDc();
[[nodiscard]] rpl::producer<bool> ShowProfileDcValue();
[[nodiscard]] bool ShowRegistrationDate();
[[nodiscard]] rpl::producer<bool> ShowRegistrationDateValue();
[[nodiscard]] bool ShowSessionDetails();
[[nodiscard]] rpl::producer<bool> ShowSessionDetailsValue();
[[nodiscard]] bool LocalNames();
[[nodiscard]] rpl::producer<bool> LocalNamesValue();
[[nodiscard]] bool HideProfileGifts();
[[nodiscard]] rpl::producer<bool> HideProfileGiftsValue();
[[nodiscard]] bool HideCreateTodo();
[[nodiscard]] rpl::producer<bool> HideCreateTodoValue();
[[nodiscard]] bool SaveProtectedContent();
[[nodiscard]] rpl::producer<bool> SaveProtectedContentValue();

} // namespace Serein::Hooks::Privacy
