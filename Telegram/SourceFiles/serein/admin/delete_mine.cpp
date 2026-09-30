#include "serein/admin/delete_mine.h"

#include "apiwrap.h"
#include "data/data_histories.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_types.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "mtproto/mtproto_response.h"
#include "ui/boxes/confirm_box.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"

namespace Serein::Admin {
namespace {

constexpr auto kPerPage = 100;

struct State {
	std::shared_ptr<Main::SessionShow> show;
	not_null<PeerData*> peer;
	MsgId offsetId = 0;
	int deleted = 0;
};

[[nodiscard]] MessageIdsList MyItemsFromTL(
		not_null<PeerData*> peer,
		const QVector<MTPMessage> &messages) {
	auto &owner = peer->owner();
	auto result = MessageIdsList();
	for (const auto &message : messages) {
		if (PeerFromMessage(message) != peer->id
			|| !DateFromMessage(message)) {
			continue;
		}
		const auto item = owner.addNewMessage(
			message,
			MessageFlags(),
			NewMessageType::Existing);
		if (item->from()->isSelf()) {
			result.push_back(item->fullId());
		}
	}
	return result;
}

void Finish(const std::shared_ptr<State> &state) {
	state->show->showToast(state->deleted
		? tr::lng_serein_delete_mine_done(
			tr::now,
			lt_amount,
			QString::number(state->deleted))
		: tr::lng_serein_delete_mine_none(tr::now));
}

void DeleteNext(std::shared_ptr<State> state) {
	const auto peer = state->peer;
	using Flag = MTPmessages_Search::Flag;
	peer->session().api().request(MTPmessages_Search(
		MTP_flags(Flag::f_from_id),
		peer->input(),
		MTP_string(),
		MTP_inputPeerSelf(),
		MTP_inputPeerEmpty(), // saved_peer_id
		MTPVector<MTPReaction>(), // saved_reaction
		MTP_int(0), // top_msg_id
		MTP_inputMessagesFilterEmpty(),
		MTP_int(0), // min_date
		MTP_int(0), // max_date
		MTP_int(state->offsetId.bare), // offset_id
		MTP_int(0), // add_offset
		MTP_int(kPerPage),
		MTP_int(0), // max_id
		MTP_int(0), // min_id
		MTP_long(0) // hash
	)).done([=](const MTPmessages_Messages &result) {
		auto &owner = peer->owner();
		const auto messages = result.match([](
				const MTPDmessages_messagesNotModified &) {
			return QVector<MTPMessage>();
		}, [&](const auto &data) {
			owner.processUsers(data.vusers());
			owner.processChats(data.vchats());
			return data.vmessages().v;
		});
		const auto mine = MyItemsFromTL(peer, messages);
		if (mine.empty()) {
			Finish(state);
			return;
		}
		state->offsetId = ranges::min(mine, ranges::less(), &FullMsgId::msg).msg;
		state->deleted += int(mine.size());
		owner.histories().deleteMessages(mine, true);
		owner.sendHistoryChangeNotifications();
		DeleteNext(state);
	}).fail([=](const MTP::Error &error) {
		MTP::ShowErrorFallback(state->show, error);
		Finish(state);
	}).send();
}

} // namespace

bool CanDeleteMyMessages(gsl::not_null<PeerData*> peer) {
	return peer->isChat() || peer->isMegagroup();
}

void ConfirmDeleteMyMessages(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto show = controller->uiShow();
	controller->show(Ui::MakeConfirmBox({
		.text = tr::lng_serein_delete_mine_sure(
			tr::now,
			lt_chat,
			peer->name()),
		.confirmed = [=](Fn<void()> &&close) {
			close();
			DeleteNext(std::make_shared<State>(State{
				.show = show,
				.peer = peer,
			}));
		},
		.confirmText = tr::lng_box_delete(),
		.confirmStyle = &st::attentionBoxButton,
	}));
}

} // namespace Serein::Admin
