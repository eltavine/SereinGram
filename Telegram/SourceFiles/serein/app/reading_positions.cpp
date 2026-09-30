#include "serein/app/reading_positions.h"

#include "serein/chats/options.h"
#include "serein/chats/reading_positions.h"
#include "serein/hooks/chats/reading_position.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "dialogs/dialogs_key.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "logs.h"
#include "main/main_session.h"
#include "window/window_session_controller.h"

namespace Serein {
namespace App {
namespace {

void SavePosition(
		gsl::not_null<Main::Session*> session,
		PeerData *peer) {
	if (!peer || !ForDevice().Get(Chats::kRememberReadingPosition)) {
		return;
	}
	const auto history = session->data().historyLoaded(peer->id);
	if (!history) {
		return;
	}
	const auto top = history->scrollTopItem;
	const auto item = top ? top->data().get() : nullptr;
	const auto message = (item && item->isRegular()) ? item->id.bare : 0;
	auto &options = ForAccount(session);
	const auto updated = Chats::SetReadingPosition(
		options.Get(Chats::kReadingPositions),
		SerializePeerId(peer->id),
		message);
	if (!options.Set(Chats::kReadingPositions, updated)) {
		LOG(("Serein Error: Could not store the reading positions."));
	}
}

} // namespace

void TrackReadingPositions(gsl::not_null<Window::SessionController*> window) {
	const auto session = &window->session();
	const auto current = window->lifetime().make_state<PeerData*>(nullptr);
	window->activeChatValue(
	) | rpl::map([](Dialogs::Key key) {
		return key.peer();
	}) | rpl::distinct_until_changed(
	) | rpl::on_next([=](PeerData *peer) {
		SavePosition(session, *current);
		*current = peer;
	}, window->lifetime());
	window->lifetime().add([=] {
		SavePosition(session, *current);
	});
}

} // namespace App

MsgId Hooks::ReadingPosition(
		gsl::not_null<Main::Session*> session,
		unsigned long long peerId,
		MsgId showAtMsgId,
		bool reopened) {
	if (reopened
		|| !peerId
		|| showAtMsgId != ShowAtUnreadMsgId
		|| !ForDevice().Get(Serein::Chats::kRememberReadingPosition)) {
		return showAtMsgId;
	}
	const auto peer = PeerId(PeerIdHelper(BareId(peerId)));
	const auto history = session->data().historyLoaded(peer);
	if (!history
		|| history->scrollTopItem
		|| !history->unreadCountKnown()
		|| history->unreadCount() > 0) {
		return showAtMsgId;
	}
	const auto message = Serein::Chats::FindReadingPosition(
		ForAccount(session).Get(Serein::Chats::kReadingPositions),
		SerializePeerId(peer));
	return message ? MsgId(message) : showAtMsgId;
}

} // namespace Serein
