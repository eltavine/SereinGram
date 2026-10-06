#pragma once

#include <gsl/pointers>

class PeerData;

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Jump {

void ShowJumpBox(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);

} // namespace Serein::Jump
