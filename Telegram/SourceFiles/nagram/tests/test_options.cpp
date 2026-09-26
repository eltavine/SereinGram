#include "nagram/core/options.h"
#include "nagram/core/exchange.h"
#include "nagram/core/device_options.h"
#include "nagram/messages/options.h"
#include "nagram/chats/options.h"
#include "nagram/compose/options.h"
#include "nagram/media/options.h"
#include "nagram/menu/model.h"
#include "nagram/privacy/options.h"
#include "nagram/messages/time_format.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

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
	using Menu::ActionId;
	using Menu::Visibility;
	const auto hiddenReply = Menu::WriteVisibility(
		{}, ActionId::Reply, Visibility::Hide);
	Require(Menu::ValidateConfig(hiddenReply), "menu config valid");
	Require(Menu::ReadVisibility(hiddenReply, ActionId::Reply)
		== Visibility::Hide, "menu hidden state");
	Require(Menu::ReadVisibility(hiddenReply, ActionId::Edit)
		== Visibility::Show, "menu default state");
	const auto optionReply = Menu::WriteVisibility(
		hiddenReply, ActionId::Reply, Visibility::WithOption);
	Require(!Menu::Visible(Menu::ReadVisibility(optionReply, ActionId::Reply),
		false), "menu option released");
	Require(Menu::Visible(Menu::ReadVisibility(optionReply, ActionId::Reply),
		true), "menu option held");
	Require(Menu::WriteVisibility(optionReply, ActionId::Reply,
		Visibility::Show).isEmpty(), "menu default removes stored override");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{"E99":"hide"}})"),
		"unknown menu action accepted");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{"E01":"unknown"}})"),
		"invalid menu state accepted");
	Require(Menu::ReadVisibility({}, ActionId::Repeat) == Visibility::Hide,
		"new menu action default");
	const auto shownRepeat = Menu::WriteVisibility({}, ActionId::Repeat,
		Visibility::Show);
	Require(Menu::ReadVisibility(shownRepeat, ActionId::Repeat)
		== Visibility::Show, "new menu action override");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{},"extra":1})"),
		"unknown menu field accepted");
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
	Require(chats.All().size() == 13, "chat option count");
	Require(chats.HasFlag(Chats::kCompactList.key,
		Flag::RefreshDialogList), "compact list refresh flag");
	Require(chats.HasFlag(Chats::kPreviewLines.key,
		Flag::RefreshDialogList), "preview line refresh flag");
	Require(!chats.HasFlag(Chats::kHideStories.key,
		Flag::RefreshDialogList), "stories use widget refresh");
	auto compose = Registry();
	Compose::RegisterOptions(compose);
	Require(compose.All().size() == 21, "compose option count");
	for (const auto &entry : compose.All()) {
		if (entry.key != Compose::kDisableEmojiHover.key
			&& entry.key != Compose::kDisableAttachHover.key
			&& entry.key != Compose::kBotCommandsToDraft.key
			&& entry.key != Compose::kInputPlaceholderMode.key
			&& entry.key != Compose::kConfirmSticker.key
			&& entry.key != Compose::kConfirmGif.key
			&& entry.key != Compose::kPreviewVoice.key
			&& entry.key != Compose::kPreviewRoundVideo.key
			&& entry.key != Compose::kConfirmPrivateCall.key
			&& entry.key != Compose::kForwardBeforeComment.key) {
			Require(compose.HasFlag(entry.key, Flag::RefreshComposeButtons),
				"compose button refresh flag");
		}
	}
	auto media = Registry();
	Media::RegisterOptions(media);
	Require(media.All().size() == 9, "media option count");
	Require(media.HasFlag(Media::kStickerScale.key,
		Flag::RefreshMessageView), "sticker scale refresh flag");
	Require(!media.HasFlag(Media::kRecentStickerLimit.key,
		Flag::RefreshMessageView), "recent sticker uses panel refresh");
	Require(media.HasFlag(Media::kDisableVideoAutoplay.key,
		Flag::RefreshMessageView), "video autoplay refresh flag");
	auto privacy = Registry();
	Privacy::RegisterOptions(privacy);
	Require(privacy.All().size() == 6, "privacy option count");
	Require(Privacy::kProfileIdFormat.validate(0)
		&& Privacy::kProfileIdFormat.validate(1)
		&& Privacy::kProfileIdFormat.validate(2)
		&& !Privacy::kProfileIdFormat.validate(3),
		"profile id format bounds");
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

	auto exchangeRegistry = Registry();
	Require(exchangeRegistry.Add(option), "exchange int registration");
	Require(exchangeRegistry.Add(accountOption), "exchange account registration");
	Menu::RegisterOptions(exchangeRegistry);
	auto exchangePrefs = MemoryPrefs();
	auto exchangeOptions = Options(exchangePrefs);
	Require(exchangeOptions.Set(option, 42), "exchange int setup");
	Require(exchangeOptions.Set(Menu::kMenuConfig, hiddenReply),
		"exchange menu setup");
	const auto exported = Exchange::Export(exchangeOptions, exchangeRegistry);
	Require(exported.invalidKeys.empty(), "unexpected export error");
	const auto exportedValues = QJsonDocument::fromJson(exported.data)
		.object().value("options").toObject();
	Require(exportedValues.value("nagram.testPercent") == 42,
		"integer missing from export");
	Require(exportedValues.value("nagram.messageMenu").isObject(),
		"structured menu missing from export");
	Require(!exportedValues.contains("nagram.testAccount"),
		"account value included in device export");

	const auto payload = QByteArray(R"({"version":1,"options":{"nagram.testPercent":25,"nagram.messageMenu":{"version":1,"states":{"E01":"option"}},"nagram.future":true}})");
	const auto plan = Exchange::PlanImport(
		exchangeOptions, exchangeRegistry, payload);
	Require(plan.error.isEmpty() && plan.changes.size() == 2,
		"valid import preview");
	Require(plan.skippedKeys == QStringList{ QString::fromLatin1("nagram.future") },
		"unknown key was not skipped");
	auto seen = std::vector<std::pair<int, Menu::Visibility>>();
	auto exchangeLifetime = rpl::lifetime();
	exchangeOptions.changes() | rpl::on_next([&](auto) {
		seen.emplace_back(exchangeOptions.Get(option),
			Menu::ReadVisibility(exchangeOptions.Get(Menu::kMenuConfig),
				ActionId::Reply));
	}, exchangeLifetime);
	const auto applied = Exchange::Apply(
		exchangeOptions, exchangeRegistry, plan);
	Require(applied.applied && applied.error.isEmpty(), "valid import apply");
	Require(exchangeOptions.Get(option) == 25 && seen.size() == 2,
		"import values or notifications");
	for (const auto &[number, visibility] : seen) {
		Require(number == 25 && visibility == Visibility::WithOption,
			"partial import observed");
	}

	const auto invalidPayload = QByteArray(R"({"version":1,"options":{"nagram.testPercent":99,"nagram.messageMenu":{"version":1,"states":{"E01":"bad"}}}})");
	const auto invalid = Exchange::PlanImport(
		exchangeOptions, exchangeRegistry, invalidPayload);
	Require(!invalid.error.isEmpty() && invalid.changes.empty(),
		"invalid object produced an import plan");
	Require(exchangeOptions.Get(option) == 25,
		"invalid import changed storage");
	Require(!Exchange::PlanImport(exchangeOptions, exchangeRegistry,
		R"({"version":"1","options":{}})").error.isEmpty(),
		"string version accepted");
	Require(!Exchange::PlanImport(exchangeOptions, exchangeRegistry,
		R"({"version":1,"options":{"nagram.testPercent":25.5}})")
		.error.isEmpty(), "fractional integer accepted");
	const auto stale = Exchange::PlanImport(exchangeOptions, exchangeRegistry,
		R"({"version":1,"options":{"nagram.testPercent":30}})");
	Require(exchangeOptions.Set(option, 20), "conflict setup");
	Require(!Exchange::Apply(exchangeOptions, exchangeRegistry, stale).applied,
		"stale import applied");
	Require(exchangeOptions.Get(option) == 20, "stale import changed storage");
	std::cout << "PASS: Nagram settings exchange" << std::endl;
}
