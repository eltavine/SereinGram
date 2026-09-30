#pragma once

#include <gsl/pointers>

class PeerData;

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Admin {

[[nodiscard]] bool CanUpgradeToSupergroup(gsl::not_null<PeerData*> peer);
void ConfirmUpgradeToSupergroup(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);

} // namespace Serein::Admin
