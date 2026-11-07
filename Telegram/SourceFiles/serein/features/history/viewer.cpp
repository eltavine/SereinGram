#include "serein/features/history/viewer.h"

#include "serein/features/history/backend.h"
#include "serein/features/history/viewer/saved_chats.h"
#include "serein/features/history/viewer/section.h"
#include "serein/ports/history_store.h"
#include "boxes/peer_list_box.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"

namespace Serein::HistoryFeature {
namespace {

using Kind = Serein::History::RecordKind;

[[nodiscard]] int CountRecords(
		gsl::not_null<Main::Session*> session,
		const Ports::RecordsQuery &query) {
	const auto store = StoreFor(session);
	return store ? store->count(query) : 0;
}

void ShowViewer(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer,
		MsgId messageId) {
	controller->showSection(
		std::make_shared<Viewer::Memento>(
			peer->owner().history(peer),
			Viewer::Query{ .peer = peer->id, .messageId = messageId }),
		Window::SectionShow::Way::Forward);
}

} // namespace

bool HasDeletedMessages(
		gsl::not_null<Main::Session*> session,
		gsl::not_null<PeerData*> peer) {
	return CountRecords(session, {
		.peerId = qint64(peer->id.value),
		.kind = Kind::Deleted,
	}) > 0;
}

bool HasEditHistory(
		gsl::not_null<Main::Session*> session,
		gsl::not_null<PeerData*> peer,
		qint64 messageId) {
	return (messageId > 0) && CountRecords(session, {
		.peerId = qint64(peer->id.value),
		.kind = Kind::Edited,
		.messageId = messageId,
	}) > 0;
}

void ShowDeletedMessages(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	ShowViewer(controller, peer, MsgId());
}

void ShowEditHistory(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer,
		qint64 messageId) {
	ShowViewer(controller, peer, MsgId(messageId));
}

void ShowSavedChats(gsl::not_null<Window::SessionController*> controller) {
	controller->show(Box<PeerListBox>(
		std::make_unique<Viewer::SavedChatsController>(controller),
		[](not_null<PeerListBox*> box) {
			box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		}));
}

void ConfirmClearHistory(
		gsl::not_null<Window::SessionController*> controller,
		PeerData *peer,
		std::function<void()> done) {
	const auto session = &controller->session();
	const auto peerId = peer ? qint64(peer->id.value) : qint64(0);
	controller->show(Ui::MakeConfirmBox({
		.text = (peer
			? tr::lng_serein_history_clear_chat_sure()
			: tr::lng_serein_history_clear_all_sure()),
		.confirmed = [=](Fn<void()> &&close) {
			const auto cleared = ClearHistory(session, peerId);
			close();
			controller->uiShow()->showToast(cleared
				? tr::lng_serein_history_cleared(tr::now)
				: tr::lng_serein_history_clear_failed(tr::now));
			if (cleared && done) {
				done();
			}
		},
		.confirmText = tr::lng_serein_history_clear(),
		.confirmStyle = &st::attentionBoxButton,
	}));
}

} // namespace Serein::HistoryFeature
