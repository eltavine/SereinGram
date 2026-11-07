#pragma once

#include <gsl/pointers>

class HistoryItem;
class PeerData;

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Inspector {

void ShowMessageDetails(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<HistoryItem*> item);
void ShowPeerDetails(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);

} // namespace Serein::Inspector
