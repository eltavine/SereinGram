#pragma once

#include <gsl/pointers>

class PeerData;

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein {

struct ChatSettingsContext {
	gsl::not_null<Window::SessionController*> controller;
	gsl::not_null<PeerData*> peer;
	gsl::not_null<Ui::VerticalLayout*> container;
};

// A row adds nothing when its feature is off or does not apply to the chat.
using ChatSettingsRow = void (*)(const ChatSettingsContext &context);

} // namespace Serein
