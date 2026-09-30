#include "serein/hooks/app.h"

#include "serein/app/modules.h"
#include "core/application.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"

#include <set>

namespace Serein::Hooks {
namespace {

void SessionStarted(gsl::not_null<Main::Session*> session) {
	for (const auto &module : App::Modules()) {
		if (module.sessionStarted) {
			module.sessionStarted(session);
		}
	}
}

void TrackAccounts() {
	static auto tracked = std::set<Main::Account*>();
	for (const auto &[index, account] : Core::App().domain().accounts()) {
		const auto raw = account.get();
		if (!tracked.emplace(raw).second) {
			continue;
		}
		raw->lifetime().add([=] { tracked.erase(raw); });
		raw->sessionValue(
		) | rpl::filter([](Main::Session *session) {
			return session != nullptr;
		}) | rpl::on_next([](Main::Session *session) {
			SessionStarted(session);
		}, raw->lifetime());
	}
}

} // namespace

void OnWindowStarted(gsl::not_null<Window::SessionController*> window) {
	for (const auto &module : App::Modules()) {
		if (module.windowStarted) {
			module.windowStarted(window);
		}
	}
}

void OnApplicationStarted() {
	static auto lifetime = rpl::lifetime();
	for (const auto &module : App::Modules()) {
		if (module.started) {
			module.started();
		}
	}
	TrackAccounts();
	Core::App().domain().accountsChanges(
	) | rpl::on_next(TrackAccounts, lifetime);
}

} // namespace Serein::Hooks
