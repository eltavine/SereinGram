#pragma once

#include <QtCore/QtGlobal>
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
[[nodiscard]] bool HasEditHistory(
	gsl::not_null<Main::Session*> session,
	gsl::not_null<PeerData*> peer,
	qint64 messageId);
void ShowDeletedMessages(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);
void ShowEditHistory(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer,
	qint64 messageId);
void ShowSavedChats(gsl::not_null<Window::SessionController*> controller);
void ConfirmClearHistory(
	gsl::not_null<Window::SessionController*> controller,
	PeerData *peer,
	std::function<void()> done = nullptr);

} // namespace Serein::HistoryFeature
