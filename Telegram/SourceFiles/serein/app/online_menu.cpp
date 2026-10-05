#include "serein/app/online_menu.h"

#include "serein/core/options.h"
#include "serein/features/ghost/model/policy.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/interface.h"
#include "base/unixtime.h"
#include "data/data_changes.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::App {
namespace {

[[nodiscard]] QString HiddenSignals(const Ghost::Policy &policy) {
	auto hidden = QStringList();
	if (policy.hideOnline) {
		hidden.push_back(tr::lng_serein_online_signal_online(tr::now));
	}
	if (policy.hideReadReceipts) {
		hidden.push_back(tr::lng_serein_online_signal_read(tr::now));
	}
	if (policy.hideTyping) {
		hidden.push_back(tr::lng_serein_online_signal_typing(tr::now));
	}
	if (policy.hideStoryViews) {
		hidden.push_back(tr::lng_serein_online_signal_stories(tr::now));
	}
	return hidden.join(u", "_q);
}

[[nodiscard]] QString OwnStatus(gsl::not_null<Main::Session*> session) {
	const auto policy = Ghost::Read(ForAccount(session), ForDevice());
	const auto invisible = policy.enabled && policy.hideOnline;
	const auto online = !invisible
		&& session->user()->lastseen().isOnline(base::unixtime::now());
	auto result = online
		? tr::lng_serein_online_self_visible(tr::now)
		: tr::lng_serein_online_self_offline(tr::now);
	if (ForDevice().Get(Interface::kOnlineStatusDetailed) && policy.enabled) {
		if (const auto hidden = HiddenSignals(policy); !hidden.isEmpty()) {
			result += u" \u00B7 "_q
				+ tr::lng_serein_online_self_hidden(tr::now, lt_list, hidden);
		}
	}
	return result;
}

} // namespace

void AddOwnOnlineStatus(
		gsl::not_null<Window::SessionController*> controller,
		const Hooks::MainMenuAction &addAction) {
	if (!ForDevice().Get(Interface::kOnlineStatusSelf)) {
		return;
	}
	const auto session = &controller->session();
	auto &account = ForAccount(session);
	auto text = rpl::merge(
		session->changes().peerFlagsValue(
			session->user(),
			Data::PeerUpdate::Flag::OnlineStatus) | rpl::to_empty,
		account.changes() | rpl::to_empty,
		ForDevice().changes() | rpl::to_empty
	) | rpl::map([=] {
		return OwnStatus(session);
	}) | rpl::distinct_until_changed();
	addAction(std::move(text), { &st::menuIconWhenOnline });
}

} // namespace Serein::App
