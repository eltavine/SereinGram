#include "serein/features/jump/box.h"

#include "serein/features/jump/model/target.h"
#include "apiwrap.h"
#include "core/click_handler_types.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"

namespace Serein::Jump {
namespace {

[[nodiscard]] bool SameChat(
		gsl::not_null<PeerData*> peer,
		const Target &target) {
	if (target.channelId) {
		return peer->isChannel()
			&& (qint64(peerToChannel(peer->id).bare) == target.channelId);
	} else if (target.username.isEmpty()) {
		return true;
	}
	return ranges::any_of(peer->usernames(), [&](const QString &name) {
		return !name.compare(target.username, Qt::CaseInsensitive);
	});
}

void JumpBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer) {
	box->setTitle(tr::lng_serein_quick_jump());
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		tr::lng_serein_quick_jump_placeholder()));
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_quick_jump_about(),
		st::boxLabel));
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto busy = box->lifetime().make_state<bool>(false);
	const auto open = [=](not_null<HistoryItem*> item) {
		box->closeBox();
		controller->showMessage(item, Window::SectionShow::Way::Forward);
	};
	const auto submit = [=] {
		if (*busy) {
			return;
		}
		const auto text = field->getLastText().trimmed();
		const auto target = ParseTarget(text);
		if (!target) {
			field->showError();
			return;
		} else if (!SameChat(peer, *target)) {
			box->closeBox();
			HiddenUrlClickHandler::Open(
				text,
				QVariant::fromValue(ClickHandlerContext{
					.sessionWindow = base::make_weak(controller.get()),
				}));
			return;
		}
		const auto id = MsgId(target->messageId);
		if (const auto item = peer->owner().message(peer->id, id)) {
			open(item);
			return;
		}
		*busy = true;
		peer->session().api().requestMessageData(peer, id, crl::guard(box, [=] {
			*busy = false;
			if (const auto item = peer->owner().message(peer->id, id)) {
				open(item);
			} else {
				field->showError();
				box->showToast(tr::lng_serein_quick_jump_missing(tr::now));
			}
		}));
	};
	field->submits() | rpl::on_next([=](auto) { submit(); }, field->lifetime());
	box->addButton(tr::lng_serein_quick_jump_go(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void ShowJumpBox(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	controller->show(Box(JumpBox, controller.get(), peer.get()));
}

} // namespace Serein::Jump
