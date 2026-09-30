#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Hooks {

void OnApplicationStarted();
void OnWindowStarted(gsl::not_null<Window::SessionController*> window);

} // namespace Serein::Hooks
