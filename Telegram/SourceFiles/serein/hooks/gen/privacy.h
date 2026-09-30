// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include <rpl/producer.h>

namespace Serein::Hooks::Privacy {

[[nodiscard]] bool DemoMode();
[[nodiscard]] rpl::producer<bool> DemoModeValue();
[[nodiscard]] bool HideReadTime();
[[nodiscard]] rpl::producer<bool> HideReadTimeValue();
[[nodiscard]] bool HideSharePhonePrompt();
[[nodiscard]] rpl::producer<bool> HideSharePhonePromptValue();
[[nodiscard]] int ProfileIdFormat();
[[nodiscard]] rpl::producer<int> ProfileIdFormatValue();
[[nodiscard]] bool ShowProfileDc();
[[nodiscard]] rpl::producer<bool> ShowProfileDcValue();
[[nodiscard]] bool HideProfileGifts();
[[nodiscard]] rpl::producer<bool> HideProfileGiftsValue();
[[nodiscard]] bool HideCreateTodo();
[[nodiscard]] rpl::producer<bool> HideCreateTodoValue();
[[nodiscard]] bool SaveProtectedContent();
[[nodiscard]] rpl::producer<bool> SaveProtectedContentValue();

} // namespace Serein::Hooks::Privacy
