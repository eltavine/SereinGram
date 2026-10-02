#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Chats {

void TrackRecentChats(gsl::not_null<Window::SessionController*> window);
void ShowRecentChats(gsl::not_null<Window::SessionController*> window);

} // namespace Serein::Chats
