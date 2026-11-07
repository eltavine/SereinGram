#include "serein/features/history/viewer/saved_chats.h"

#include "serein/features/history/backend.h"
#include "serein/features/history/viewer.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "window/window_session_controller.h"

namespace Serein::HistoryFeature::Viewer {
namespace {

class Row final : public PeerListRow {
public:
	using PeerListRow::PeerListRow;

	QString generateName() override {
		const auto name = PeerListRow::generateName();
		return name.isEmpty()
			? tr::lng_serein_history_unknown_chat(
				tr::now,
				lt_id,
				QString::number(peerToBareMTPInt(peer()->id).v))
			: name;
	}
	QString generateShortName() override {
		return generateName();
	}

};

} // namespace

SavedChatsController::SavedChatsController(
	not_null<Window::SessionController*> window)
: _window(window) {
}

Main::Session &SavedChatsController::session() const {
	return _window->session();
}

void SavedChatsController::prepare() {
	delegate()->peerListSetTitle(tr::lng_serein_history_saved_chats());
	delegate()->peerListSetSearchMode(PeerListSearchMode::Enabled);
	const auto store = StoreFor(&session());
	const auto peers = store
		? store->peersWithDeleted(std::nullopt)
		: std::vector<Ports::PeerSummary>();
	for (const auto &summary : peers) {
		const auto peer = session().data().peer(
			PeerId(PeerIdHelper(BareId(summary.peerId))));
		auto row = std::make_unique<Row>(peer);
		row->setCustomStatus(tr::lng_serein_history_deleted_count(
			tr::now,
			lt_amount,
			QString::number(summary.count)));
		delegate()->peerListAppendRow(std::move(row));
	}
	if (peers.empty()) {
		setDescriptionText(tr::lng_serein_history_saved_chats_empty(tr::now));
	}
	delegate()->peerListRefreshRows();
}

void SavedChatsController::rowClicked(not_null<PeerListRow*> row) {
	const auto peer = row->peer();
	_window->hideLayer();
	ShowDeletedMessages(_window, peer);
}

} // namespace Serein::HistoryFeature::Viewer
