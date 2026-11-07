#include "serein/chats/chat_settings.h"

#include "serein/chats/local_pins.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/chats.h"
#include "data/data_peer.h"
#include "data/data_peer_id.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <algorithm>

namespace Serein::Chats {

void ShowChatSettings(
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer,
		std::span<const ChatSettingsRow> rows) {
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_chat_settings());
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			rpl::single(peer->name()),
			st::boxDividerLabel));
		const auto container = box->addRow(
			object_ptr<Ui::VerticalLayout>(box),
			style::margins());
		for (const auto row : rows) {
			row({
				.controller = controller,
				.peer = peer,
				.container = container,
			});
		}
		if (!container->count()) {
			box->addRow(object_ptr<Ui::FlatLabel>(
				box,
				tr::lng_serein_chat_settings_empty(),
				st::boxLabel));
		}
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

void AddLocalPinRow(const ChatSettingsContext &context) {
	if (!ForDevice().Get(kLocalPinning)) {
		return;
	}
	const auto session = &context.controller->session();
	const auto id = SerializePeerId(context.peer->id);
	const auto pinned = [=] {
		const auto pins = ParseLocalPins(ForAccount(session).Get(kLocalPins));
		return std::ranges::find(pins, id) != pins.end();
	};
	const auto button = context.container->add(
		object_ptr<Ui::SettingsButton>(
			context.container,
			tr::lng_serein_chat_local_pin(),
			st::settingsButtonNoIcon));
	const auto state = button->lifetime().make_state<rpl::variable<bool>>(
		pinned());
	button->toggleOn(state->value());
	button->toggledChanges(
	) | rpl::filter([=](bool value) {
		return value != pinned();
	}) | rpl::on_next([=](bool value) {
		auto &options = ForAccount(session);
		const auto count = ParseLocalPins(options.Get(kLocalPins)).size();
		if (value && int(count) >= kLocalPinsLimit) {
			context.controller->uiShow()->showToast(
				tr::lng_serein_local_pin_limit(tr::now));
			state->force_assign(false);
			return;
		}
		Expects(options.Set(
			kLocalPins,
			ToggleLocalPin(options.Get(kLocalPins), id)));
		*state = value;
	}, button->lifetime());
}

} // namespace Serein::Chats
