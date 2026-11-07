#pragma once

#include <gsl/pointers>

namespace Main { class Session; }

namespace Window { class SessionController; }
namespace Ui::Menu { struct MenuCallback; }
class PeerData;

namespace Serein::Privacy {

void StartHiddenAccounts();
void WatchHiddenChats(gsl::not_null<Main::Session*> session);
void ShowHiddenChats(gsl::not_null<Window::SessionController*> controller);
void ShowHiddenAccounts(gsl::not_null<Window::SessionController*> controller);
void AddHideChatAction(
	const Ui::Menu::MenuCallback &addAction,
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);

} // namespace Serein::Privacy
