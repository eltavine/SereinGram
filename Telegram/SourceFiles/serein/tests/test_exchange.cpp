#include "serein/core/exchange.h"
#include "base/basic_types.h"
#include "serein/tests/memory_prefs.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <algorithm>

namespace {

using namespace Serein;

constexpr auto kDeviceFlag = Option<bool>{
	"serein.testDeviceFlag",
	Scope::Device,
	false,
	Category::Interface,
	"lng_serein_test_device_flag" };
constexpr auto kAccountFlag = Option<bool>{
	"serein.testAccountFlag",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_test_account_flag" };
constexpr auto kAccountState = Option<int>{
	"serein.testAccountState",
	Scope::Account,
	0,
	Category::Chats,
	"lng_serein_test_account_state",
	static_cast<unsigned>(Flag::Hidden) };

[[nodiscard]] Registry MakeRegistry() {
	auto result = Registry();
	Require(result.Add(kDeviceFlag), "device option registration");
	Require(result.Add(kAccountFlag), "account option registration");
	Require(result.Add(kAccountState), "hidden account option registration");
	return result;
}

[[nodiscard]] QJsonObject Root(const QByteArray &data) {
	return QJsonDocument::fromJson(data).object();
}

struct Stores {
	Tests::MemoryPrefs devicePrefs;
	Tests::MemoryPrefs accountPrefs;
	Options device = Options(devicePrefs);
	Options account = Options(accountPrefs, Scope::Account);
};

} // namespace

TEST_CASE("AccountExchange") {
	const auto registry = MakeRegistry();
	auto source = Stores();
	Require(source.device.Set(kDeviceFlag, true), "device setup");
	const auto deviceOnly = Root(
		Exchange::Export(source.device, &source.account, registry).data);
	Require(deviceOnly.value("version") == 1 && !deviceOnly.contains("account"),
		"a device-only export changed its format");

	Require(source.account.Set(kAccountFlag, true)
		&& source.account.Set(kAccountState, 7), "account setup");
	const auto exported = Exchange::Export(
		source.device,
		&source.account,
		registry);
	const auto root = Root(exported.data);
	const auto account = root.value("account").toObject();
	Require(exported.invalidKeys.isEmpty(), "valid values reported invalid");
	Require(root.value("version") == 2, "account export kept version 1");
	Require(account.value("serein.testAccountFlag") == true,
		"account value missing from export");
	Require(!account.contains("serein.testAccountState"),
		"internal account state exported");
	Require(!root.value("options").toObject().contains("serein.testAccountFlag"),
		"account value written to the device section");

	auto target = Stores();
	const auto plan = Exchange::PlanImport(
		target.device,
		&target.account,
		registry,
		exported.data);
	Require(plan.error.isEmpty() && plan.changes.size() == 2,
		"account import preview");
	Require(std::ranges::any_of(plan.changes, [](const ExchangeChange &change) {
		return change.scope == Scope::Account
			&& change.key == u"serein.testAccountFlag"_q;
	}), "account change lost its scope");
	Require(Exchange::Apply(target.device, &target.account, registry, plan).applied
		&& target.device.Get(kDeviceFlag)
		&& target.account.Get(kAccountFlag), "account import apply");

	auto deviceTarget = Stores();
	const auto withoutAccount = Exchange::PlanImport(
		deviceTarget.device,
		nullptr,
		registry,
		exported.data);
	Require(withoutAccount.error.isEmpty()
		&& withoutAccount.changes.size() == 1
		&& withoutAccount.skippedKeys
			== QStringList{ u"serein.testAccountFlag"_q },
		"account section applied without an account");

	const auto misplaced = Exchange::PlanImport(
		deviceTarget.device,
		&deviceTarget.account,
		registry,
		R"({"version":2,"options":{"serein.testAccountFlag":true},"account":{"serein.testDeviceFlag":true,"serein.testAccountState":3}})");
	Require(misplaced.error.isEmpty() && misplaced.changes.empty()
		&& misplaced.skippedKeys.size() == 3, "misplaced keys imported");

	for (const auto &unsupported : {
			QByteArray(R"({"version":1,"options":{},"account":{}})"),
			QByteArray(R"({"version":2,"options":{},"extra":1})"),
			QByteArray(R"({"version":2,"options":{},"account":[]})"),
			QByteArray(R"({"version":3,"options":{}})") }) {
		Require(!Exchange::PlanImport(
			deviceTarget.device,
			&deviceTarget.account,
			registry,
			unsupported).error.isEmpty(), "unsupported document accepted");
	}
	Require(!Exchange::PlanImport(
		deviceTarget.device,
		&deviceTarget.account,
		registry,
		R"({"version":2,"options":{},"account":{"serein.testAccountFlag":1}})"
	).error.isEmpty(), "invalid account value accepted");

	const auto stale = Exchange::PlanImport(
		deviceTarget.device,
		&deviceTarget.account,
		registry,
		R"({"version":2,"options":{},"account":{"serein.testAccountFlag":true}})");
	Require(deviceTarget.account.Set(kAccountFlag, true), "conflict setup");
	Require(!Exchange::Apply(
		deviceTarget.device,
		&deviceTarget.account,
		registry,
		stale).applied, "stale account change applied");

	const auto reset = Exchange::PlanReset(
		source.device,
		&source.account,
		registry);
	Require(reset.error.isEmpty() && reset.changes.size() == 2,
		"reset preview misses a store");
	Require(Exchange::Apply(source.device, &source.account, registry, reset).applied
		&& !source.device.Get(kDeviceFlag)
		&& !source.account.Get(kAccountFlag)
		&& source.account.Get(kAccountState) == 7,
		"reset touched internal state or missed a value");

	Require(!Exchange::PlanImport(
		source.account,
		nullptr,
		registry,
		R"({"version":1,"options":{}})").error.isEmpty(),
		"account store accepted as the device store");
	Require(!Exchange::PlanImport(
		source.device,
		&source.device,
		registry,
		R"({"version":1,"options":{}})").error.isEmpty(),
		"device store accepted as the account store");
	Require(Exchange::Transferable(*registry.Find("serein.testAccountFlag"))
		&& !Exchange::Transferable(*registry.Find("serein.testAccountState")),
		"transferable options misreported");
}
