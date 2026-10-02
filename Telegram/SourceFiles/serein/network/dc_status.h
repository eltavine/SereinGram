#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Network {

void ShowDatacenterStatus(gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Network
