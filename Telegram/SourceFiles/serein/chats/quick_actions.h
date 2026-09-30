#pragma once

#include <gsl/pointers>

class PeerData;

namespace Ui::Menu {
struct MenuCallback;
} // namespace Ui::Menu

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Chats {

void FillQuickActions(
	const Ui::Menu::MenuCallback &addAction,
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);

} // namespace Serein::Chats
