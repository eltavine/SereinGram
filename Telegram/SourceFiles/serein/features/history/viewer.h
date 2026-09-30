#pragma once

#include <gsl/pointers>

#include <functional>

class PeerData;

namespace Main {
class Session;
} // namespace Main

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::HistoryFeature {

[[nodiscard]] bool HasDeletedMessages(
	gsl::not_null<Main::Session*> session,
	gsl::not_null<PeerData*> peer);
void ShowDeletedMessages(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);
void ConfirmClearHistory(
	gsl::not_null<Window::SessionController*> controller,
	PeerData *peer,
	std::function<void()> done = nullptr);

} // namespace Serein::HistoryFeature
