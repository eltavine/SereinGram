#include "serein/hooks/interface/main_menu.h"

#include "serein/app/online_menu.h"
#include "serein/chats/recent_tracking.h"
#include "serein/privacy/hidden.h"
#include "serein/privacy/options.h"
#include "serein/privacy/network.h"
#include "mtproto/mtp_instance.h"
#include "serein/features/stories/composer.h"
#include "serein/hooks/ghost.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/schema/gen/settings/media.h"
#include "data/data_user.h"
#include "info/downloads/info_downloads_widget.h"
#include "info/info_memento.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Hooks {

void FillMainMenu(
		gsl::not_null<Window::SessionController*> controller,
		const MainMenuAction &addAction) {
	App::AddOwnOnlineStatus(controller, addAction);
	const auto networkEnabled = controller->session().mtp().networkEnabled();
	if (ForDevice().Get(Serein::Privacy::kAccountNetworkControls) || !networkEnabled) {
		addAction(networkEnabled
			? tr::lng_serein_disconnect_account() : tr::lng_serein_reconnect_account(),
			{ &st::menuIconNetwork }
		)->setClickedCallback([=] { Serein::Privacy::ToggleAccountNetwork(controller); });
	}
	if (ForDevice().Get(Serein::Privacy::kHiddenChatsEnabled)) {
		addAction(tr::lng_serein_hidden_chats(), { &st::menuIconArchive }
		)->setClickedCallback([=] { Serein::Privacy::ShowHiddenChats(controller); });
	}
	if (Serein::Privacy::HiddenAccountsAvailable()) {
		addAction(tr::lng_serein_hidden_accounts(), { &st::menuIconLock }
		)->setClickedCallback([=] { Serein::Privacy::ShowHiddenAccounts(controller); });
	}
	addAction(tr::lng_downloads_section(), { &st::menuIconDownload }
	)->setClickedCallback([=] { controller->showSection(Info::Downloads::Make(controller->session().user())); });
	if (ForDevice().Get(Serein::Media::kStoryPosting)) {
		addAction(
			tr::lng_serein_story_new(),
			{ &st::menuIconStoriesSavedSection }
		)->setClickedCallback([=] {
			Serein::Stories::StartPosting(
				controller->uiShow(),
				controller->session().user());
		});
	}
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
		Serein::Chats::ShowRecentChats(controller);
	});
}

} // namespace Serein::Hooks
