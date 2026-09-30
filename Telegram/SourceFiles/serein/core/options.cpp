#include "serein/core/options.h"
#include "serein/core/device_options.h"
#include "serein/interface/options.h"
#include "serein/chats/options.h"
#include "serein/compose/options.h"
#include "serein/media/options.h"
#include "serein/menu/model.h"
#include "serein/privacy/options.h"
#include "serein/privacy/alias.h"
#include "serein/messages/options.h"
#include "serein/services/model.h"
#include "serein/filters/model.h"
#include "serein/links/model.h"
#include "serein/snapshot/snapshot.h"
#include "serein/features/ghost/model/policy.h"
#include "serein/features/history/model/recorder.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "main/main_session.h"
#include "storage/storage_account.h"

#include <map>
#include <memory>

namespace Storage {

template <>
std::optional<QByteArray> Account::readPrefImpl<QByteArray>(
		std::string_view key) {
	return readPrefGeneric(key);
}

template <>
void Account::writePrefImpl<QByteArray>(
		std::string_view key,
		QByteArray value) {
	writePrefGeneric(key, value);
}

} // namespace Storage

namespace Serein {

Options &ForDevice() {
	static auto prefs = DevicePrefs(Core::App().settings());
	return details::SharedDeviceOptions(prefs);
}

DevicePrefs::DevicePrefs(Core::Settings &settings) : _settings(settings) { }

QByteArray DevicePrefs::read(std::string_view key) {
	return _settings.readPref<QByteArray>(key);
}

void DevicePrefs::write(std::string_view key, const QByteArray &value) {
	_settings.writePref<QByteArray>(key, value);
}

void DevicePrefs::clear(std::string_view key) {
	_settings.clearPref(key);
}

AccountPrefs::AccountPrefs(Storage::Account &account) : _account(account) { }

QByteArray AccountPrefs::read(std::string_view key) {
	return _account.readPref<QByteArray>(key);
}

void AccountPrefs::write(std::string_view key, const QByteArray &value) {
	_account.writePref<QByteArray>(key, value);
}

void AccountPrefs::clear(std::string_view key) {
	_account.clearPref(key);
}

Options &ForAccount(gsl::not_null<Main::Session*> session) {
	struct State {
		explicit State(Storage::Account &account)
		: prefs(account), options(prefs, Scope::Account) { }
		AccountPrefs prefs;
		Options options;
	};
	static auto states = std::map<Main::Session*, std::unique_ptr<State>>();
	const auto found = states.find(session);
	if (found != states.end()) {
		return found->second->options;
	}
	const auto inserted = states.emplace(
		session, std::make_unique<State>(session->local())).first;
	session->lifetime().add([session] { states.erase(session); });
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
		RegisterServiceOptions(result);
		Ghost::RegisterOptions(result);
		HistorySettings::RegisterOptions(result);
		return result;
	}();
	return registry;
}

} // namespace Serein
