#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::App {

void TrackReadingPositions(gsl::not_null<Window::SessionController*> window);

} // namespace Serein::App
