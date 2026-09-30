#include "serein/features/history/viewer.h"

#include "serein/hooks/history.h"
#include "serein/ports/history_store.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"

namespace Serein::HistoryFeature {
namespace {

constexpr auto kDeletedLimit = 100;

[[nodiscard]] std::vector<History::Record> DeletedFor(
		not_null<Main::Session*> session,
		PeerId peer,
		int limit) {
	const auto store = Hooks::HistoryStoreFor(session);
	if (!store) {
		return {};
	}
	return store->deleted({
		.peerId = qint64(peer.value),
		.limit = limit,
	});
}

[[nodiscard]] QString Describe(
		not_null<Main::Session*> session,
		const std::vector<History::Record> &records) {
	auto blocks = QStringList();
	for (const auto &record : records) {
		auto header = langDateTimeFull(
			base::unixtime::parse(TimeId(record.date)));
		if (record.fromPeerId) {
			const auto from = session->data().peerLoaded(
				PeerId(PeerIdHelper(BareId(record.fromPeerId))));
			if (from) {
				header = from->name() + u", "_q + header;
			}
		}
		const auto body = record.text.isEmpty()
			? record.mediaSummary
			: record.text;
		blocks.push_back(header + u"\n"_q + body);
	}
	return blocks.join(u"\n\n"_q);
}

} // namespace

bool HasDeletedMessages(
		gsl::not_null<Main::Session*> session,
		gsl::not_null<PeerData*> peer) {
	return !DeletedFor(session, peer->id, 1).empty();
}

void ShowDeletedMessages(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto session = &controller->session();
	const auto records = DeletedFor(session, peer->id, kDeletedLimit);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_menu_deleted_messages());
		const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			(records.empty()
				? tr::lng_serein_history_empty(tr::now)
				: Describe(session, records)),
			st::boxLabel));
		label->setSelectable(true);
		if (!records.empty()) {
			box->addLeftButton(tr::lng_serein_history_clear_chat(), [=] {
				ConfirmClearHistory(controller, peer, crl::guard(box, [=] {
					box->closeBox();
				}));
			});
		}
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
			const auto store = Hooks::HistoryStoreFor(session);
			const auto cleared = !store
				|| (peerId ? store->clearPeer(peerId) : store->clearAll());
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
