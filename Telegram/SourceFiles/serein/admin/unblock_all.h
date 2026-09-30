#pragma once

#include <gsl/pointers>

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Admin {

void ConfirmUnblockAll(gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Admin
