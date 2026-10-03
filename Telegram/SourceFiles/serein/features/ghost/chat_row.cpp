#include "serein/features/ghost/chat_row.h"

#include "serein/core/options.h"
#include "serein/features/ghost/model/exceptions.h"
#include "serein/hooks/ghost.h"
#include "serein/schema/gen/settings/ghost.h"
#include "data/data_histories.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_settings.h"

namespace Serein::Ghost {

void AddReadReceiptRow(const ChatSettingsContext &context) {
	const auto session = &context.controller->session();
	const auto peer = context.peer;
	const auto excepted = [=] {
		return HasException(
			ForAccount(session).Get(kReadReceiptExceptions),
			peer->id.value);
	};
	if (!excepted() && Hooks::AllowReadReceipt(session)) {
		return;
	}
	const auto button = context.container->add(
		object_ptr<Ui::SettingsButton>(
			context.container,
			tr::lng_serein_chat_read_receipts(),
			st::settingsButtonNoIcon));
	button->toggleOn(rpl::single(excepted()));
	button->toggledChanges(
	) | rpl::filter([=](bool value) {
		return value != excepted();
	}) | rpl::on_next([=](bool value) {
		auto &options = ForAccount(session);
		Expects(options.Set(
			kReadReceiptExceptions,
			ToggleException(
				options.Get(kReadReceiptExceptions),
				peer->id.value)));
		if (value) {
			session->data().histories().readInbox(
				session->data().history(peer));
		}
	}, button->lifetime());
}

} // namespace Serein::Ghost
