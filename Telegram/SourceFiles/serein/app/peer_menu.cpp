#include "serein/hooks/peer_menu.h"

#include "serein/hooks/privacy/alias.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Hooks {

void FillHistoryMenu(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	if (topic) {
		return;
	}
	addAction(tr::lng_serein_jump_to_beginning(tr::now), [=] {
		controller->showPeerHistory(
			peer,
			Window::SectionShow::Way::Forward,
			MsgId(1));
	}, &st::menuIconShowInChat);
}

void FillProfileMenu(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	if (topic || peer->isSelf() || peer->migrateTo()) {
		return;
	}
	const auto show = controller->uiShow();
	addAction(tr::lng_serein_peer_alias(tr::now), [=] {
		Privacy::ShowAlias(show, peer);
	}, &st::menuIconEdit);
}

} // namespace Serein::Hooks
