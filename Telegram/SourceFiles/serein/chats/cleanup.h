#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Chats {

void ShowChatCleanup(gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Chats
