#include "serein/admin/unblock_all.h"

#include "api/api_blocked_peers.h"
#include "apiwrap.h"
#include "base/flat_set.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "ui/boxes/confirm_box.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"

namespace Serein::Admin {
namespace {

using Slice = Api::BlockedPeers::Slice;

struct State {
	std::shared_ptr<Main::SessionShow> show;
	not_null<Main::Session*> session;
	base::flat_set<PeerId> unblocked;
};

void Finish(const std::shared_ptr<State> &state, bool failed) {
	state->show->showToast(failed
		? tr::lng_serein_unblock_all_failed(
			tr::now,
			lt_amount,
			QString::number(state->unblocked.size()))
		: tr::lng_serein_unblock_all_done(
			tr::now,
			lt_amount,
			QString::number(state->unblocked.size())));
}

void RequestSlice(std::shared_ptr<State> state);

void UnblockFrom(
		std::shared_ptr<State> state,
		std::shared_ptr<Slice> slice,
		int index) {
	if (index >= slice->list.size()) {
		RequestSlice(state);
		return;
	}
	const auto id = slice->list[index].id;
	const auto peer = state->session->data().peerLoaded(id);
	if (!peer || state->unblocked.contains(id)) {
		Finish(state, true);
		return;
	}
	state->session->api().blockedPeers().unblock(peer, [=](bool success) {
		if (!success) {
			Finish(state, true);
			return;
		}
		state->unblocked.emplace(id);
		UnblockFrom(state, slice, index + 1);
	}, true);
}

void RequestSlice(std::shared_ptr<State> state) {
	state->session->api().blockedPeers().request(0, [=](Slice slice) {
		if (slice.list.isEmpty()) {
			Finish(state, false);
			return;
		}
		UnblockFrom(state, std::make_shared<Slice>(std::move(slice)), 0);
	});
}

} // namespace

void ConfirmUnblockAll(gsl::not_null<Window::SessionController*> controller) {
	const auto show = controller->uiShow();
	const auto session = &controller->session();
	controller->show(Ui::MakeConfirmBox({
		.text = tr::lng_serein_unblock_all_sure(tr::now),
		.confirmed = [=](Fn<void()> &&close) {
			close();
			RequestSlice(std::make_shared<State>(State{
				.show = show,
				.session = session,
			}));
		},
		.confirmText = tr::lng_serein_unblock_all_confirm(),
		.confirmStyle = &st::attentionBoxButton,
	}));
}

} // namespace Serein::Admin
