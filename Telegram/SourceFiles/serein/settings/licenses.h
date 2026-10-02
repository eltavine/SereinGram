#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein {

void ShowLicenses(gsl::not_null<Window::SessionController*> controller);

} // namespace Serein
