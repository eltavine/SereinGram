#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Chats {

void TrackReadingPositions(gsl::not_null<Window::SessionController*> window);

} // namespace Serein::Chats
