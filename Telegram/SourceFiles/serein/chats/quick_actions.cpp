#include "serein/chats/quick_actions.h"

#include "serein/chats/options.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "dialogs/dialogs_key.h"
#include "history/history.h"
#include "history/view/history_view_pinned_section.h"
#include "info/info_controller.h"
#include "info/info_memento.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "storage/storage_shared_media.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Chats {

void FillQuickActions(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	if (!ForDevice().Get(kChatQuickActions)) {
		return;
	}
	const auto history = peer->owner().history(peer);
	addAction(tr::lng_serein_quick_search(tr::now), [=] {
		controller->searchInChat(Dialogs::Key(history));
	}, &st::menuIconSearch);
	addAction(tr::lng_serein_quick_media(tr::now), [=] {
		controller->showSection(std::make_shared<Info::Memento>(
			peer,
			Info::Section(Storage::SharedMediaType::Photo)));
	}, &st::menuIconPhoto);
	addAction(tr::lng_serein_quick_pinned(tr::now), [=] {
		controller->showSection(
			std::make_shared<HistoryView::PinnedMemento>(history));
	}, &st::menuIconPin);
}

} // namespace Serein::Chats
