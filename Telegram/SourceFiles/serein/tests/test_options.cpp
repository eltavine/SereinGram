#include "serein/core/options.h"
#include "base/basic_types.h"
#include "serein/core/exchange.h"
#include "serein/core/device_options.h"
#include "serein/interface/options.h"
#include "serein/hooks/interface/main_menu.h"
#include "serein/messages/options.h"
#include "serein/chats/options.h"
#include "serein/compose/options.h"
#include "serein/media/options.h"
#include "serein/menu/model.h"
#include "serein/privacy/options.h"
#include "serein/filters/model.h"
#include "serein/hooks/services/model.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/hooks/messages/time_format.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace {

class MemoryPrefs final : public Serein::RawPrefs {
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
	using namespace Serein;
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
	const auto hiddenRating = Menu::WriteVisibility(
		{}, ActionId::QuickRatingSecond, Visibility::Hide);
	Require(Menu::ValidateConfig(hiddenRating)
		&& Menu::ReadVisibility(hiddenRating, ActionId::QuickRatingSecond)
			== Visibility::Hide
		&& Menu::DefaultVisibility(ActionId::QuickRatingFirst)
			== Visibility::Show,
		"quick rating visibility");
	Require(Menu::kQuickRatingFirst.validate(QString(256, u'x'))
		&& !Menu::kQuickRatingFirst.validate(QString(257, u'x'))
		&& Menu::kQuickRatingSecond.fallback.isEmpty(),
		"quick rating text bounds");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{"E99":"hide"}})"),
		"unknown menu action accepted");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{"E01":"unknown"}})"),
		"invalid menu state accepted");
	Require(Menu::ValidateConfig(R"({"version":2,"states":{"E22":"option","E29":"show"}})"),
		"valid menu config rejected");
	Require(Menu::ValidateConfig(R"({"version":1,"states":{"E21":"hide"}})"),
		"version 1 menu config rejected");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{"E22":"hide"}})"),
		"version 1 menu config with E22 accepted");
	Require(!Menu::ValidateConfig(R"({"version":3,"states":{}})"),
		"unknown menu config version accepted");
	Require(!Menu::ValidateConfig(R"({"version":2,"states":{},"extra":1})"),
		"menu config with an unknown key accepted");
	Require(!Menu::ValidateConfig(R"({"version":2})"),
		"menu config without states accepted");
	Require(!Menu::ValidateConfig(R"({"version":2,"states":[]})"),
		"menu config with array states accepted");
	Require(!Menu::ValidateConfig(R"({"version":2,"states":{"E01":1}})"),
		"menu config with a numeric state accepted");
	Require(!Menu::ValidateConfig("{broken"), "broken menu config accepted");
	Require(Menu::ReadVisibility({}, ActionId::Repeat) == Visibility::Hide,
		"new menu action default");
	Require(Menu::ReadVisibility({}, ActionId::Reading) == Visibility::Hide,
		"reading menu action default");
	const auto oldReading = QByteArray(
		R"({"version":1,"states":{"E21":"show","E23":"show"}})");
	Require(Menu::ValidateConfig(oldReading), "old menu config valid");
	Require(Menu::ReadVisibility(oldReading, ActionId::Reading)
		== Visibility::Show, "old reading visibility");
	Require(Menu::ReadVisibility(oldReading, ActionId::Screenshot)
		== Visibility::Hide, "old config screenshot default");
	const auto migrated = Menu::WriteVisibility(
		oldReading, ActionId::Screenshot, Visibility::Show);
	Require(Menu::ReadVisibility(migrated, ActionId::Reading)
		== Visibility::Show, "reading visibility migrated");
	Require(Menu::ReadVisibility(migrated, ActionId::Screenshot)
		== Visibility::Show, "screenshot visibility saved");
	Require(Menu::ReadVisibility(migrated, ActionId::FilterAuthor)
		== Visibility::Show, "filter author visibility preserved");
	const auto shownRepeat = Menu::WriteVisibility({}, ActionId::Repeat,
		Visibility::Show);
	Require(Menu::ReadVisibility(shownRepeat, ActionId::Repeat)
		== Visibility::Show, "new menu action override");
	Require(!Menu::ValidateConfig(R"({"version":1,"states":{},"extra":1})"),
		"unknown menu field accepted");
	const auto option = Option<int>{
		"serein.testPercent", Scope::Device, 0, Category::Interface,
		"lng_serein_test_percent", 0, ValidPercent };
	auto registry = Registry();
	Require(registry.Add(option), "register option");
	Require(!registry.Add(option), "duplicate key accepted");
	Require(registry.All().size() == 1, "registry count");
	auto messages = Registry();
	Messages::RegisterOptions(messages);
	Require(messages.All().size() == 36, "message option count");
	Require(Messages::kFadeDeletedMessages.fallback, "deleted messages not faded by default");
	auto refreshCount = 0;
	for (const auto &entry : messages.All()) {
		Require(entry.scope == Scope::Device, "message option scope");
		Require(entry.category == Category::Messages, "message option category");
		refreshCount += messages.HasFlag(entry.key, Flag::RefreshMessageView);
	}
	Require(refreshCount == 23, "message refresh option count");
	Require(Messages::kReadingChinese.validate(0)
		&& Messages::kReadingChinese.validate(1)
		&& Messages::kReadingChinese.validate(2)
		&& !Messages::kReadingChinese.validate(3),
		"reading conversion modes");
	Require(messages.HasFlag(Messages::kSecondsInMessages.key,
		Flag::RefreshMessageView), "message refresh option missing");
	Require(!messages.HasFlag(Messages::kHideReactionMenu.key,
		Flag::RefreshMessageView), "unrelated refresh option");
	Require(!messages.HasFlag("serein.unknown", Flag::RefreshMessageView),
		"unknown refresh option");
	auto chats = Registry();
	Chats::RegisterOptions(chats);
	Require(chats.All().size() == 27, "chat option count");
	Require(!Chats::kLocalPinning.fallback
		&& Chats::kLocalPins.scope == Scope::Account
		&& Chats::kLocalPins.validate(QString::fromLatin1("7,9"))
		&& !Chats::kLocalPins.validate(QString::fromLatin1("7,7")),
		"local pinning must be an opt-in with account scoped pins");
	Require(!Chats::kManagementShortcuts.fallback, "management shortcuts must be opt-in");
	Require(!Chats::kChatQuickActions.fallback, "chat quick actions must be opt-in");
	Require(!Chats::kRememberReadingPosition.fallback
		&& Chats::kRememberReadingPosition.scope == Scope::Device
		&& Chats::kReadingPositions.scope == Scope::Account
		&& Chats::kReadingPositions.validate(QString::fromLatin1("7:150,9:200"))
		&& !Chats::kReadingPositions.validate(QString::fromLatin1("7:0")),
		"reading positions must be an opt-in with account scoped storage");
	Require(Chats::kHiddenFolderIds.scope == Scope::Account
		&& Chats::kHiddenFolderIds.validate(QString::fromLatin1("2,5"))
		&& !Chats::kHiddenFolderIds.validate(QString::fromLatin1("5,2")),
		"hidden folder ids");
	Require(Chats::kManagedFolderIds.scope == Scope::Account
		&& Chats::kManagedFolderIds.validate(QString::fromLatin1("1,3,8"))
		&& !Chats::kManagedFolderIds.validate(QString::fromLatin1("3,1"))
		&& !Chats::kManagedFolderIds.validate(QString::fromLatin1("1,1")),
		"managed folder ids must be account scoped and unique");
	Require(Chats::kChatSort.validate(0)
		&& Chats::kChatSort.validate(0xE41)
		&& !Chats::kChatSort.validate(1)
		&& !Chats::kChatSort.validate(0x1000),
		"chat sort encoding");
	Require(Chats::kStartupFolderMode.scope == Scope::Account
		&& Chats::kStartupFolderId.scope == Scope::Account
		&& Chats::kLastOpenedFolderId.scope == Scope::Account,
		"startup folder must be account scoped");
	Require(chats.HasFlag(Chats::kCompactList.key,
		Flag::RefreshDialogList), "compact list refresh flag");
	Require(chats.HasFlag(Chats::kPreviewLines.key,
		Flag::RefreshDialogList), "preview line refresh flag");
	Require(!chats.HasFlag(Chats::kHideStories.key,
		Flag::RefreshDialogList), "stories use widget refresh");
	auto interface = Registry();
	Interface::RegisterOptions(interface);
	Require(interface.All().size() == 19, "interface option count");
	Require(!Interface::kMoreAccounts.fallback
		&& Interface::kMoreAccounts.scope == Scope::Device,
		"more accounts must be a device opt-in");
	Require(interface.HasFlag(Interface::kHalfwidthUiPunctuation.key,
		Flag::RequiresRestart), "interface text restart flag");
	Require(interface.HasFlag(Interface::kBubbleRoundness.key,
		Flag::RequiresRestart), "roundness restart flag");
	Require(Interface::kAvatarRoundness.validate(0)
		&& Interface::kAvatarRoundness.validate(10)
		&& Interface::kAvatarRoundness.validate(100)
		&& !Interface::kAvatarRoundness.validate(9)
		&& !Interface::kAvatarRoundness.validate(101),
		"avatar roundness bounds");
	Require(Interface::kTextMessageWidth.validate(0)
		&& Interface::kTextMessageWidth.validate(50)
		&& Interface::kTextMessageWidth.validate(400)
		&& !Interface::kTextMessageWidth.validate(49)
		&& !Interface::kTextMessageWidth.validate(401),
		"text width bounds");
	Require(interface.HasFlag(Interface::kHideReplyThumbnail.key,
		Flag::RefreshMessageView), "reply thumbnail refresh flag");
	Require(Interface::ValidMainMenuBytes({}), "default menu config");
	Require(Interface::ValidMainMenuBytes(
			Interface::SerializeMainMenuConfig(Interface::MainMenuDefaults()))
		&& Interface::MainMenuDefaults().seasonalDecorations,
		"default menu config invalid");
	Require(!Interface::ValidMainMenuBytes(R"({"version":1,"order":[],"hidden":["settings"],"title":"","seasonalDecorations":true})"),
		"settings cannot be hidden");
	Require(!Interface::ValidMainMenuBytes(R"({"version":1,"order":["calls","calls"],"hidden":[],"title":"","seasonalDecorations":true})"),
		"duplicate menu action accepted");
	const auto menu = [](const QString &changes) {
		auto value = QJsonDocument::fromJson(Interface::SerializeMainMenuConfig(
			Interface::MainMenuDefaults())).object();
		const auto patch = QJsonDocument::fromJson(changes.toUtf8()).object();
		for (auto i = patch.begin(); i != patch.end(); ++i) {
			if (i.value().isNull()) {
				value.remove(i.key());
			} else {
				value.insert(i.key(), i.value());
			}
		}
		return Interface::ValidMainMenuBytes(
			QJsonDocument(value).toJson(QJsonDocument::Compact));
	};
	Require(menu(u"{\"hidden\":[\"calls\"],\"order\":[\"nightMode\",\"profile\"]}"_q),
		"valid menu config rejected");
	Require(!menu(u"{\"order\":[\"unknown\"]}"_q), "unknown menu action accepted");
	Require(!menu(u"{\"order\":[7]}"_q), "numeric menu action accepted");
	Require(!menu(u"{\"title\":\"%1\"}"_q.arg(QString(97, u'x'))),
		"long menu title accepted");
	Require(menu(u"{\"title\":\"%1\"}"_q.arg(QString(96, u'x'))),
		"menu title at the limit rejected");
	Require(!menu(u"{\"title\":\"a\\nb\"}"_q), "menu title with a newline accepted");
	Require(!menu(u"{\"seasonalDecorations\":null}"_q), "menu without a key accepted");
	Require(!menu(u"{\"extra\":1}"_q), "menu with an unknown key accepted");
	Require(!menu(u"{\"version\":2}"_q), "unknown menu version accepted");
	Require(Interface::kNotificationDelay.validate(0)
		&& Interface::kNotificationDelay.validate(500)
		&& Interface::kNotificationDelay.validate(60000)
		&& !Interface::kNotificationDelay.validate(400)
		&& !Interface::kNotificationDelay.validate(61000),
		"notification delay bounds");
	auto compose = Registry();
	Compose::RegisterOptions(compose);
	Require(compose.All().size() == 35, "compose option count");
	Require(Compose::kMentionMenu.key == "serein.mentionMenu"
		&& Compose::kMentionMenu.scope == Scope::Device
		&& !Compose::kMentionMenu.fallback
		&& !Messages::kShowChannelBadge.fallback,
		"UI additions must stay off until enabled");
	Require(Compose::kDefaultCodeLanguage.validate(QString::fromLatin1("cpp"))
		&& !Compose::kDefaultCodeLanguage.validate(QString::fromLatin1("c++!")),
		"code language validation");
	Require(Compose::kQuickReplies.validate(
		R"({"version":1,"replies":["one","two"]})")
		&& !Compose::kQuickReplies.validate(
			R"({"version":1,"replies":["one"]})")
		&& !Compose::kQuickReplies.validate(
			R"({"version":2,"replies":["one","two"]})")
		&& !Compose::kQuickReplies.validate(
			R"({"version":1,"replies":["one",2]})")
		&& !Compose::kQuickReplies.validate(
			R"({"version":1,"replies":["one","two"],"extra":1})")
		&& !Compose::kQuickReplies.validate(R"({"version":1})")
		&& !Compose::kQuickReplies.validate("not json"),
		"quick reply schema");
	for (const auto &entry : compose.All()) {
		if (entry.key != Compose::kDisableEmojiHover.key
			&& entry.key != Compose::kDisableAttachHover.key
			&& entry.key != Compose::kBotCommandsToDraft.key
			&& entry.key != Compose::kInputPlaceholderMode.key
			&& entry.key != Compose::kDisableAutoMarkdown.key
			&& entry.key != Compose::kDisableLinkPreview.key
			&& entry.key != Compose::kSpaceOnSend.key
			&& entry.key != Compose::kSpaceOnEdit.key
			&& entry.key != Compose::kDefaultCodeLanguage.key
			&& entry.key != Compose::kQuickReplies.key
			&& entry.key != Compose::kConfirmSticker.key
			&& entry.key != Compose::kConfirmGif.key
			&& entry.key != Compose::kPreviewVoice.key
			&& entry.key != Compose::kPreviewRoundVideo.key
			&& entry.key != Compose::kConfirmPrivateCall.key
			&& entry.key != Compose::kForwardBeforeComment.key
			&& entry.key != Compose::kSendSilently.key
			&& entry.key != Compose::kMentionMenu.key
			&& entry.key != Compose::kFormatToolbar.key
			&& entry.key != Compose::kDraftTranslation.key
			&& entry.key != Compose::kCaptionAboveMedia.key
			&& entry.key != Compose::kRememberForwardOptions.key
			&& entry.key != Compose::kLastForwardOptions.key
			&& entry.key != Compose::kTextReplacements.key) {
			Require(compose.HasFlag(entry.key, Flag::RefreshComposeButtons),
				"compose button refresh flag");
		}
	}
	auto media = Registry();
	Media::RegisterOptions(media);
	Require(media.All().size() == 17, "media option count");
	Require(media.HasFlag(Media::kStickerScale.key,
		Flag::RefreshMessageView), "sticker scale refresh flag");
	Require(!media.HasFlag(Media::kRecentStickerLimit.key,
		Flag::RefreshMessageView), "recent sticker uses panel refresh");
	Require(media.HasFlag(Media::kDisableVideoAutoplay.key,
		Flag::RefreshMessageView), "video autoplay refresh flag");
	auto privacy = Registry();
	Privacy::RegisterOptions(privacy);
	Require(privacy.All().size() == 12, "privacy option count");
	Require(!Privacy::kAutoDemoMode.fallback
		&& Privacy::kAutoDemoMode.scope == Scope::Device
		&& privacy.HasFlag(Privacy::kAutoDemoMode.key, Flag::Exportable),
		"automatic streamer mode must be an exportable device opt-in");
	Require(!Privacy::kSaveProtectedContent.fallback
		&& privacy.HasFlag(Privacy::kSaveProtectedContent.key, Flag::Exportable),
		"protected content saving must be an exportable opt-in");
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
	Require(prefs.values["serein.testPercent"] == "42", "stored value");
	Require(options.Get(option) == 42, "round trip");
	Require(options.Set(option, 42), "repeat write");
	Require(changes == (std::vector{ 0, 42 }), "duplicate notification");
	Require(!options.Set(option, 101), "invalid value accepted");
	Require(options.Get(option) == 42, "invalid value changed storage");

	Require(options.Set(option, 0), "clear to default");
	Require(!prefs.values.contains("serein.testPercent"), "default not cleared");
	Require(options.Get(option) == 0, "cached value not refreshed by write");
	Require(changes == (std::vector{ 0, 42, 0 }), "clear notification");

	prefs.values["serein.testPercent"] = "broken";
	auto reloaded = Options(prefs);
	Require(reloaded.Get(option) == 0, "invalid stored value fallback");
	Require(prefs.values["serein.testPercent"] == "broken",
		"invalid payload overwritten");
	Require(reloaded.invalidKeys().contains(option.key), "read error absent");
	Require(reloaded.Set(option, 0), "clear invalid value");
	Require(!prefs.values.contains("serein.testPercent"), "invalid value not cleared");
	Require(reloaded.invalidKeys().empty(), "stale read error");

	const auto text = Option<QString>{
		"serein.testText", Scope::Device, QString::fromUtf8("default"),
		Category::Interface, "lng_serein_test_text" };
	Require(options.Set(text, QString()), "empty string write");
	Require(options.Get(text).isEmpty(), "empty string round trip");
	Require(!options.Set(text, QString::fromUtf8("two\nlines")),
		"multiline string accepted");
	std::cout << "PASS: Serein device options" << std::endl;

	const auto accountOption = Option<bool>{
		"serein.testAccount", Scope::Account, false, Category::Chats,
		"lng_serein_test_account" };
	auto firstPrefs = MemoryPrefs();
	auto secondPrefs = MemoryPrefs();
	auto first = Options(firstPrefs, Scope::Account);
	auto second = Options(secondPrefs, Scope::Account);
	Require(first.Set(accountOption, true), "account write");
	Require(first.Get(accountOption), "first account value");
	Require(!second.Get(accountOption), "account values leaked");
	Require(!options.Set(accountOption, true), "device accepted account option");
	const auto accountScoped = std::array{
		Chats::kRecentChats.scope,
		Chats::kManagedFolderIds.scope,
		Chats::kStartupFolderId.scope,
		Filters::kRules.scope,
		Ghost::kGhostMode.scope,
		Ghost::kGhostHideReadReceipts.scope,
		Ghost::kGhostHideStoryViews.scope,
		Ghost::kGhostHideOnline.scope,
		Ghost::kGhostHideTyping.scope,
		Ghost::kGhostUseScheduledMessages.scope,
		HistorySettings::kHistorySaveDeleted.scope,
		HistorySettings::kHistoryKeepDeletedInPlace.scope,
		HistorySettings::kHistoryKeepExpiredMedia.scope,
		HistorySettings::kHistorySaveEdits.scope,
		HistorySettings::kHistoryExcludedPeers.scope,
	};
	Require(std::ranges::all_of(accountScoped, [](Scope scope) {
		return scope == Scope::Account;
	}), "per-account data stored for the whole device");
	Require(ServiceSettings::kProxySubscription.validate(QString())
		&& ServiceSettings::kProxySubscription.validate(
			QString::fromLatin1("https://example.com/proxies.txt"))
		&& !ServiceSettings::kProxySubscription.validate(
			QString::fromLatin1("http://example.com/proxies.txt"))
		&& !ServiceSettings::kProxySubscription.validate(
			QString::fromLatin1("https://a b")),
		"proxy subscription accepts only HTTPS addresses");
	const auto deviceScoped = std::array{
		Privacy::kDemoMode.scope,
		Messages::kFadeDeletedMessages.scope,
		Messages::kShowChannelBadge.scope,
		kServicesConfig.scope,
	};
	Require(std::ranges::all_of(deviceScoped, [](Scope scope) {
		return scope == Scope::Device;
	}), "device preference stored per account");
	std::cout << "PASS: Serein account options" << std::endl;

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
	Require(exportedValues.value("serein.testPercent") == 42,
		"integer missing from export");
	Require(exportedValues.value("serein.messageMenu").isObject(),
		"structured menu missing from export");
	Require(!exportedValues.contains("serein.testAccount"),
		"account value included in device export");

	const auto payload = QByteArray(R"({"version":1,"options":{"serein.testPercent":25,"serein.messageMenu":{"version":1,"states":{"E01":"option"}},"serein.future":true}})");
	const auto plan = Exchange::PlanImport(
		exchangeOptions, exchangeRegistry, payload);
	Require(plan.error.isEmpty() && plan.changes.size() == 2,
		"valid import preview");
	Require(plan.skippedKeys == QStringList{ QString::fromLatin1("serein.future") },
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

	const auto invalidPayload = QByteArray(R"({"version":1,"options":{"serein.testPercent":99,"serein.messageMenu":{"version":1,"states":{"E01":"bad"}}}})");
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
		R"({"version":1,"options":{"serein.testPercent":25.5}})")
		.error.isEmpty(), "fractional integer accepted");
	const auto stale = Exchange::PlanImport(exchangeOptions, exchangeRegistry,
		R"({"version":1,"options":{"serein.testPercent":30}})");
	Require(exchangeOptions.Set(option, 20), "conflict setup");
	Require(!Exchange::Apply(exchangeOptions, exchangeRegistry, stale).applied,
		"stale import applied");
	Require(exchangeOptions.Get(option) == 20, "stale import changed storage");
	const auto reset = Exchange::PlanReset(exchangeOptions, exchangeRegistry);
	Require(reset.error.isEmpty() && reset.changes.size() == 2,
		"reset preview misses a changed device setting");
	Require(Exchange::Apply(exchangeOptions, exchangeRegistry, reset).applied
		&& exchangeOptions.Get(option) == option.fallback
		&& exchangeOptions.Get(Menu::kMenuConfig).isEmpty(),
		"reset did not restore the defaults");
	Require(Exchange::PlanReset(exchangeOptions, exchangeRegistry)
		.changes.empty(), "reset left changed settings");
	std::cout << "PASS: Serein settings exchange" << std::endl;
}
