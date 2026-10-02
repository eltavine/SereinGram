#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Privacy {

void ShowQrScanner(gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Privacy
