#include "nagram/core/options.h"
#include "nagram/core/device_options.h"

#include "core/application.h"
#include "core/core_settings.h"

namespace Nagram {

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

} // namespace Nagram
