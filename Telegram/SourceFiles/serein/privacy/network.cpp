#include "serein/privacy/network.h"

#include "calls/calls_call.h"
#include "calls/calls_instance.h"
#include "calls/group/calls_group_call.h"
#include "core/application.h"
#include "data/data_user.h"
#include "data/data_document.h"
#include "inline_bots/bot_attach_web_view.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "media/audio/media_audio.h"
#include "media/player/media_player_instance.h"
#include "mtproto/mtp_instance.h"
#include "window/window_session_controller.h"

namespace Serein::Privacy {

void ToggleAccountNetwork(gsl::not_null<Window::SessionController*> controller) {
	const auto session = &controller->session();
	auto &mtp = session->mtp();
	const auto enabled = !mtp.networkEnabled();
	if (!enabled) {
		session->attachWebView().closeAll();
		auto &calls = Core::App().calls();
		if (const auto call = calls.currentCall()) {
			if (&call->user()->session() == session) call->hangup();
		}
		if (const auto call = calls.currentGroupCall()) {
			if (&call->peer()->session() == session) call->hangup();
		}
		const auto player = ::Media::Player::instance();
		for (const auto type : { AudioMsgId::Type::Song, AudioMsgId::Type::Voice }) {
			const auto document = player->getState(type).id.audio();
			if (document && &document->session() == session) {
				player->pause(type);
			}
		}
	}
	mtp.setNetworkEnabled(enabled);
	controller->showToast(enabled ? tr::lng_serein_account_network_resumed(tr::now)
		: tr::lng_serein_account_network_paused(tr::now));
}

} // namespace Serein::Privacy
