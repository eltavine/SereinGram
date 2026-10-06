#pragma once

#include <gsl/pointers>

class PeerData;

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Purge {

[[nodiscard]] bool CanDeleteMine(gsl::not_null<PeerData*> peer);
void ShowDeleteMine(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> chat);

} // namespace Serein::Purge
