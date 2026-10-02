#include "serein/chats/quick_actions.h"

#include "serein/chats/chat_cache.h"
#include "serein/chats/options.h"
#include "data/data_channel.h"
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
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <limits>

namespace Serein::Chats {
namespace {

void JumpToMessageBox(
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
	const auto submit = [=] {
		auto ok = false;
		const auto id = field->getLastText().trimmed().toLongLong(&ok);
		if (!ok || id <= 0 || id > std::numeric_limits<int32>::max()) {
			field->showError();
			return;
		}
		box->closeBox();
		controller->showPeerHistory(
			peer,
			Window::SectionShow::Way::Forward,
			MsgId(id));
	};
	field->submits() | rpl::on_next([=](auto) { submit(); }, field->lifetime());
	box->addButton(tr::lng_serein_quick_jump_go(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

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
	if (const auto channel = peer->asMegagroup()) {
		if (const auto linked = channel->discussionLink()) {
			addAction(tr::lng_profile_view_channel(tr::now), [=] {
				if (channel->invitePeekExpires()) {
					controller->showToast(
						tr::lng_channel_invite_private(tr::now));
					return;
				}
				controller->showPeerHistory(
					linked,
					Window::SectionShow::Way::Forward);
			}, &st::menuIconChannel);
		}
	}
	addAction(tr::lng_serein_quick_pinned(tr::now), [=] {
		controller->showSection(
			std::make_shared<HistoryView::PinnedMemento>(history));
	}, &st::menuIconPin);
	addAction(tr::lng_serein_quick_jump(tr::now), [=] {
		controller->show(Box(JumpToMessageBox, controller, peer));
	}, &st::menuIconShowInChat);
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
