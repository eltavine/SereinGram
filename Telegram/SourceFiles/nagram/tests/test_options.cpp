#include "nagram/core/options.h"
#include "nagram/core/device_options.h"
#include "nagram/messages/options.h"
#include "nagram/chats/options.h"
#include "nagram/messages/time_format.h"

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace {

class MemoryPrefs final : public Nagram::RawPrefs {
public:
	[[nodiscard]] QByteArray read(std::string_view key) override {
		const auto found = values.find(std::string(key));
		return (found == values.end()) ? QByteArray() : found->second;
	}
	void write(std::string_view key, const QByteArray &value) override {
		values[std::string(key)] = value;
	}
	void clear(std::string_view key) override {
		values.erase(std::string(key));
	}

	std::map<std::string, QByteArray> values;
};

void Require(bool condition, const char *message) {
	if (!condition) {
		throw std::runtime_error(message);
	}
}

[[nodiscard]] bool ValidPercent(const int &value) {
	return value >= 0 && value <= 100;
}

} // namespace

void TestOptions() {
	using namespace Nagram;
	const auto option = Option<int>{
		"nagram.testPercent", Scope::Device, 0, Category::Interface,
		"lng_nagram_test_percent", 0, ValidPercent };
	auto registry = Registry();
	Require(registry.Add(option), "register option");
	Require(!registry.Add(option), "duplicate key accepted");
	Require(registry.All().size() == 1, "registry count");
	auto messages = Registry();
	Messages::RegisterOptions(messages);
	Require(messages.All().size() == 24, "message option count");
	auto refreshCount = 0;
	for (const auto &entry : messages.All()) {
		Require(entry.scope == Scope::Device, "message option scope");
		Require(entry.category == Category::Messages, "message option category");
		refreshCount += messages.HasFlag(entry.key, Flag::RefreshMessageView);
	}
	Require(refreshCount == 17, "message refresh option count");
	Require(messages.HasFlag(Messages::kSecondsInMessages.key,
		Flag::RefreshMessageView), "message refresh option missing");
	Require(!messages.HasFlag(Messages::kHideReactionMenu.key,
		Flag::RefreshMessageView), "unrelated refresh option");
	Require(!messages.HasFlag("nagram.unknown", Flag::RefreshMessageView),
		"unknown refresh option");
	auto chats = Registry();
	Chats::RegisterOptions(chats);
	Require(chats.All().size() == 11, "chat option count");
	Require(chats.HasFlag(Chats::kCompactList.key,
		Flag::RefreshDialogList), "compact list refresh flag");
	Require(chats.HasFlag(Chats::kPreviewLines.key,
		Flag::RefreshDialogList), "preview line refresh flag");
	Require(!chats.HasFlag(Chats::kHideStories.key,
		Flag::RefreshDialogList), "stories use widget refresh");
	const auto locale = QLocale();
	QLocale::setDefault(QLocale::c());
	Require(Messages::FormatTime(QTime(9, 8, 7), false)
		== QString::fromLatin1("09:08"),
		"default time format");
	const auto withSeconds = Messages::FormatTime(QTime(9, 8, 7), true);
	if (withSeconds != QString::fromLatin1("09:08:07")) {
		throw std::runtime_error(
			"seconds time format: " + withSeconds.toStdString());
	}
	QLocale::setDefault(locale);

	auto prefs = MemoryPrefs();
	auto &subscriber = details::SharedDeviceOptions(prefs);
	auto &writer = details::SharedDeviceOptions(prefs);
	Require(&subscriber == &writer, "device entries use different instances");
	auto &options = subscriber;
	Require(options.Get(option) == 0, "default value");
	auto changes = std::vector<int>();
	auto lifetime = rpl::lifetime();
	subscriber.Value(option) | rpl::on_next([&](int value) {
		changes.push_back(value);
	}, lifetime);
	Require(changes == std::vector{ 0 }, "initial notification");
	Require(writer.Set(option, 42), "valid write");
	Require(prefs.values["nagram.testPercent"] == "42", "stored value");
	Require(options.Get(option) == 42, "round trip");
	Require(options.Set(option, 42), "repeat write");
	Require(changes == (std::vector{ 0, 42 }), "duplicate notification");
	Require(!options.Set(option, 101), "invalid value accepted");
	Require(options.Get(option) == 42, "invalid value changed storage");

	prefs.values["nagram.testPercent"] = "broken";
	Require(options.Get(option) == 0, "invalid stored value fallback");
	Require(prefs.values["nagram.testPercent"] == "broken",
		"invalid payload overwritten");
	Require(options.invalidKeys().contains(option.key), "read error absent");
	Require(options.Set(option, 0), "clear to default");
	Require(!prefs.values.contains("nagram.testPercent"), "default not cleared");
	Require(options.invalidKeys().empty(), "stale read error");
	Require(changes == (std::vector{ 0, 42, 0 }), "clear notification");

	const auto text = Option<QString>{
		"nagram.testText", Scope::Device, QString::fromUtf8("default"),
		Category::Interface, "lng_nagram_test_text" };
	Require(options.Set(text, QString()), "empty string write");
	Require(options.Get(text).isEmpty(), "empty string round trip");
	Require(!options.Set(text, QString::fromUtf8("two\nlines")),
		"multiline string accepted");
	std::cout << "PASS: Nagram device options" << std::endl;

	const auto accountOption = Option<bool>{
		"nagram.testAccount", Scope::Account, false, Category::Chats,
		"lng_nagram_test_account" };
	auto firstPrefs = MemoryPrefs();
	auto secondPrefs = MemoryPrefs();
	auto first = Options(firstPrefs, Scope::Account);
	auto second = Options(secondPrefs, Scope::Account);
	Require(first.Set(accountOption, true), "account write");
	Require(first.Get(accountOption), "first account value");
	Require(!second.Get(accountOption), "account values leaked");
	Require(!options.Set(accountOption, true), "device accepted account option");
	std::cout << "PASS: Nagram account options" << std::endl;
}
