#pragma once

#include "serein/hooks/interface/main_menu.h"

namespace Serein::App {

void AddOwnOnlineStatus(
	gsl::not_null<Window::SessionController*> controller,
	const Hooks::MainMenuAction &addAction);

} // namespace Serein::App
