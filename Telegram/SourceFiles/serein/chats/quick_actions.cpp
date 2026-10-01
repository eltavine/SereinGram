#include "serein/chats/quick_actions.h"

#include "serein/chats/chat_cache.h"
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
#include "ui/boxes/confirm_box.h"
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
	addAction(tr::lng_serein_quick_clear_cache(tr::now), [=] {
		controller->show(Ui::MakeConfirmBox({
			.text = tr::lng_serein_quick_clear_cache_confirm(),
			.confirmed = [=](Fn<void()> &&close) {
				const auto amount = ClearLoadedMediaCache(history);
				close();
				controller->uiShow()->showToast(tr::lng_serein_quick_clear_cache_done(
					tr::now,
					lt_amount,
					QString::number(amount)));
			},
			.confirmText = tr::lng_serein_quick_clear_cache(),
		}));
	}, &st::menuIconClear);
}

} // namespace Serein::Chats
