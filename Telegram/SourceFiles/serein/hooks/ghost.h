#pragma once

#include <gsl/pointers>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks {

[[nodiscard]] bool AllowOnline(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool AllowTyping(gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks
