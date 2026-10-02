#include "serein/chats/recent_tracking.h"

#include "serein/chats/options.h"
#include "serein/chats/recent.h"
#include "boxes/peer_list_box.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "dialogs/dialogs_key.h"
#include "lang/lang_keys.h"
#include "logs.h"
#include "main/main_session.h"
#include "window/window_session_controller.h"

namespace Serein::Chats {
namespace {

class RecentController final : public PeerListController {
public:
	explicit RecentController(
		gsl::not_null<Window::SessionController*> window)
	: _window(window) {
	}

	void prepare() override {
		auto &data = _window->session().data();
		const auto ids = Chats::ParseRecentChats(
			ForAccount(&_window->session()).Get(Chats::kRecentChats));
		for (const auto id : ids) {
			if (const auto peer = data.peerLoaded(DeserializePeerId(id))) {
				delegate()->peerListAppendRow(
					std::make_unique<PeerListRow>(peer));
			}
		}
		if (!delegate()->peerListFullRowsCount()) {
			setDescriptionText(tr::lng_serein_recent_chats_empty(tr::now));
		}
		delegate()->peerListRefreshRows();
	}

	void rowClicked(gsl::not_null<PeerListRow*> row) override {
		const auto peer = row->peer();
		const auto window = _window;
		delegate()->peerListUiShow()->hideLayer();
		window->showPeerHistory(peer);
	}

	Main::Session &session() const override {
		return _window->session();
	}

private:
	const gsl::not_null<Window::SessionController*> _window;

};

} // namespace

void TrackRecentChats(gsl::not_null<Window::SessionController*> window) {
	window->activeChatValue(
	) | rpl::map([](Dialogs::Key key) {
		return key.peer();
	}) | rpl::filter([](PeerData *peer) {
		return peer != nullptr;
	}) | rpl::distinct_until_changed(
	) | rpl::on_next([=](PeerData *peer) {
		auto &options = ForAccount(&window->session());
		const auto updated = Chats::PushRecentChat(
			options.Get(Chats::kRecentChats),
			SerializePeerId(peer->id));
		if (!options.Set(Chats::kRecentChats, updated)) {
			LOG(("Serein Error: Could not store the recent chats list."));
		}
	}, window->lifetime());
}

void ShowRecentChats(gsl::not_null<Window::SessionController*> window) {
	const auto session = &window->session();
	window->show(Box<PeerListBox>(
		std::make_unique<RecentController>(window),
		[=](gsl::not_null<PeerListBox*> box) {
			box->setTitle(tr::lng_serein_recent_chats());
			box->addButton(tr::lng_close(), [=] { box->closeBox(); });
			box->addLeftButton(tr::lng_serein_recent_chats_clear(), [=] {
				if (ForAccount(session).Set(Chats::kRecentChats, QString())) {
					box->closeBox();
				}
			});
		}));
}

} // namespace Serein::Chats
