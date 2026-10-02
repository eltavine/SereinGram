#pragma once

#include "serein/ports/prefs.h"

namespace Core {
class Settings;
} // namespace Core

namespace Storage {
class Account;
} // namespace Storage

namespace Serein::Adapters {

class DevicePrefs final : public RawPrefs {
public:
	explicit DevicePrefs(Core::Settings &settings);
	[[nodiscard]] QByteArray read(std::string_view key) override;
	void write(std::string_view key, const QByteArray &value) override;
	void clear(std::string_view key) override;

private:
	Core::Settings &_settings;

};

class AccountPrefs final : public RawPrefs {
public:
	explicit AccountPrefs(Storage::Account &account);
	[[nodiscard]] QByteArray read(std::string_view key) override;
	void write(std::string_view key, const QByteArray &value) override;
	void clear(std::string_view key) override;

private:
	Storage::Account &_account;

};

} // namespace Serein::Adapters
