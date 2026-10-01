#include "serein/hooks/interface/main_menu.h"

#include "serein/app/recent_chats.h"
#include "serein/hooks/ghost.h"
#include "serein/schema/gen/settings/interface.h"
#include "lang/lang_keys.h"
#include "settings/settings_common.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Hooks {

void FillMainMenu(
		gsl::not_null<Window::SessionController*> controller,
		const MainMenuAction &addAction) {
	if (!ForDevice().Get(Serein::Interface::kMenuShortcuts)) {
		return;
	}
	BindGhostToggle(
		addAction(tr::lng_serein_ghost_mode(), { &st::menuIconStealth }),
		&controller->session());
	BindStreamerToggle(
		addAction(tr::lng_serein_demo_mode(), { &st::menuIconVideoChat }));
	addAction(
		tr::lng_serein_recent_chats(),
		{ &st::menuIconTimer }
	)->setClickedCallback([=] {
		App::ShowRecentChats(controller);
	});
}

} // namespace Serein::Hooks
