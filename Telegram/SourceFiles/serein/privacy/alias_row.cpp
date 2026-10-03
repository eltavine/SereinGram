#include "serein/privacy/alias_row.h"

#include "serein/privacy/alias.h"
#include "serein/privacy/options.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "settings/settings_common.h"
#include "window/window_session_controller.h"

#include "styles/style_settings.h"

namespace Serein::Privacy {

void AddAliasRow(const ChatSettingsContext &context) {
	const auto peer = context.peer;
	if (!ForDevice().Get(kLocalNames) || peer->isSelf()) {
		return;
	}
	const auto controller = context.controller;
	::Settings::AddButtonWithLabel(
		context.container,
		tr::lng_serein_peer_alias(),
		ForAccount(&peer->session()).Value(
			kAliases
		) | rpl::map([=](const QByteArray &) { return Alias(peer); }),
		st::settingsButtonNoIcon
	)->setClickedCallback([=] {
		ShowAlias(controller->uiShow(), peer);
	});
}

} // namespace Serein::Privacy
