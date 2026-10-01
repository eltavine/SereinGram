#include "serein/app/shortcuts.h"

#include "serein/app/recent_chats.h"

#include "serein/features/ghost/model/policy.h"
#include "serein/privacy/options.h"
#include "core/application.h"
#include "core/shortcuts.h"
#include "lang/lang_keys.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "window/window_controller.h"

namespace Serein::App {
namespace {

void Announce(const QString &text) {
	if (const auto window = Core::App().activeWindow()) {
		window->showToast(text);
	}
}

bool ToggleGhostMode() {
	auto &account = Core::App().domain().active();
	if (!account.sessionExists()) {
		return false;
	}
	auto &options = ForAccount(&account.session());
	const auto enabled = !Ghost::Enabled(options, ForDevice());
	Expects(Ghost::SetEnabled(options, ForDevice(), enabled));
	Announce(enabled
		? tr::lng_serein_ghost_mode_enabled(tr::now)
		: tr::lng_serein_ghost_mode_disabled(tr::now));
	return true;
}

bool ToggleDemoMode() {
	auto &options = ForDevice();
	const auto enabled = !options.Get(Privacy::kDemoMode);
	Expects(options.Set(Privacy::kDemoMode, enabled));
	Announce(enabled
		? tr::lng_serein_demo_mode_enabled(tr::now)
		: tr::lng_serein_demo_mode_disabled(tr::now));
	return true;
}

bool ShowRecent() {
	const auto window = Core::App().activeWindow();
	const auto controller = window ? window->sessionController() : nullptr;
	if (!controller) {
		return false;
	}
	ShowRecentChats(controller);
	return true;
}

} // namespace

void StartShortcuts() {
	static auto lifetime = rpl::lifetime();
	Shortcuts::Requests(
	) | rpl::on_next([](not_null<Shortcuts::Request*> request) {
		using Command = Shortcuts::Command;
		request->check(Command::SereinToggleGhostMode)
			&& request->handle(ToggleGhostMode);
		request->check(Command::SereinToggleDemoMode)
			&& request->handle(ToggleDemoMode);
		request->check(Command::SereinRecentChats)
			&& request->handle(ShowRecent);
	}, lifetime);
}

} // namespace Serein::App
