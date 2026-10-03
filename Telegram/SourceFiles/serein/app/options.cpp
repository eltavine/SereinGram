#include "serein/core/options.h"

#include "serein/adapters/tdesktop/prefs.h"
#include "serein/core/device_options.h"
#include "serein/chats/options.h"
#include "serein/compose/options.h"
#include "serein/features/ghost/model/policy.h"
#include "serein/features/history/model/recorder.h"
#include "serein/filters/model.h"
#include "serein/hooks/services/model.h"
#include "serein/interface/options.h"
#include "serein/links/model.h"
#include "serein/media/options.h"
#include "serein/menu/model.h"
#include "serein/messages/options.h"
#include "serein/privacy/alias.h"
#include "serein/privacy/options.h"
#include "serein/snapshot/snapshot.h"
#include "core/application.h"
#include "main/main_account.h"
#include "main/main_session.h"

#include <map>
#include <memory>

namespace Serein {

Options &ForDevice() {
	static auto prefs = Adapters::DevicePrefs(Core::App().settings());
	return details::SharedDeviceOptions(prefs);
}

Options &ForAccount(gsl::not_null<Main::Session*> session) {
	struct State {
		explicit State(Storage::Account &account)
		: prefs(account), options(prefs, Scope::Account) {
		}
		Adapters::AccountPrefs prefs;
		Options options;
		rpl::lifetime lifetime;
	};
	static auto states = std::map<Main::Session*, std::unique_ptr<State>>();
	const auto found = states.find(session);
	if (found != states.end()) {
		return found->second->options;
	}
	const auto inserted = states.emplace(
		session, std::make_unique<State>(session->local())).first;
	// Reachable from inside Session(), before its lifetime is constructed.
	session->account().sessionValue(
	) | rpl::filter([=](Main::Session *value) {
		return (value == session.get());
	}) | rpl::take(1) | rpl::on_next([=] {
		session->lifetime().add([session] { states.erase(session); });
	}, inserted->second->lifetime);
	return inserted->second->options;
}

const Registry &RegisteredOptions() {
	static const auto registry = [] {
		auto result = Registry();
		Chats::RegisterOptions(result);
		Interface::RegisterOptions(result);
		Compose::RegisterOptions(result);
		Media::RegisterOptions(result);
		Menu::RegisterOptions(result);
		Privacy::RegisterOptions(result);
		Privacy::RegisterAliasOptions(result);
		Messages::RegisterOptions(result);
		Filters::RegisterOptions(result);
		Links::RegisterOptions(result);
		Snapshot::RegisterOptions(result);
		ServiceSettings::RegisterOptions(result);
		Ghost::RegisterOptions(result);
		HistorySettings::RegisterOptions(result);
		return result;
	}();
	return registry;
}

} // namespace Serein
