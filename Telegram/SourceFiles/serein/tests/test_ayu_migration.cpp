#include "serein/settings/ayu_migration.h"

#include "base/basic_types.h"
#include "serein/core/exchange.h"
#include "serein/tests/full_registry.h"
#include "serein/tests/memory_prefs.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QJsonDocument>

namespace {

using namespace Serein;

[[nodiscard]] QJsonObject Parse(const QByteArray &bytes) {
	return QJsonDocument::fromJson(bytes).object();
}

TEST_CASE("AyuSettingsImportThroughExchange") {
	const auto source = Parse(R"json({
		"ghostModeSettings": {
			"0": { "sendReadMessages": true },
			"12345678901": { "sendReadMessages": false, "sendReadStories": true },
			"987654321": { "sendOnlinePackets": false, "sendUploadProgress": false }
		},
		"useGlobalGhostMode": false,
		"saveDeletedMessages": true,
		"showAttachButtonInMessageField": false,
		"showPeerId": 2,
		"monoFont": "Menlo",
		"shadowBanIds": [12345678901],
		"translationProvider": 1
	})json");
	const auto migrated = MigrateAyuSettings(source, 12345678901);
	Require(!migrated.settings.isEmpty(), "Ayu settings were not converted");
	Require(migrated.skippedKeys == QStringList{
		u"shadowBanIds"_q, u"translationProvider"_q,
	}, "unsupported keys were not reported");
	Require(!migrated.settings.contains("12345678901")
		&& !migrated.settings.contains("987654321"),
		"source account IDs leaked into the settings backup");

	auto devicePrefs = Tests::MemoryPrefs();
	auto accountPrefs = Tests::MemoryPrefs();
	auto device = Options(devicePrefs);
	auto account = Options(accountPrefs, Scope::Account);
	const auto registry = Tests::FullRegistry();
	const auto plan = Exchange::PlanImport(
		device, &account, registry, migrated.settings);
	Require(plan.error.isEmpty() && plan.skippedKeys.isEmpty(),
		"converted options must be transferable");
	Require(Exchange::Apply(device, &account, registry, plan).applied,
		"converted options must apply through the normal exchange");
	Require(account.Get(Ghost::kGhostMode)
		&& account.Get(Ghost::kGhostHideReadReceipts)
		&& !account.Get(Ghost::kGhostHideStoryViews)
		&& !account.Get(Ghost::kGhostHideOnline)
		&& !account.Get(Ghost::kGhostHideTyping),
		"ghost settings came from the wrong profile");
	Require(!device.Get(Ghost::kGhostAllAccounts),
		"import enabled ghost mode for other accounts");
	Require(account.Get(HistorySettings::kHistorySaveDeleted)
		&& device.Get(Compose::kHideAttachButton)
		&& device.Get(Privacy::kProfileIdFormat) == 1
		&& device.Get(Interface::kMonospaceFont) == u"Menlo"_q,
		"device or current-account preferences failed to migrate");
}

TEST_CASE("AyuSettingsImportGlobalAndLegacyProfiles") {
	const auto legacy = MigrateAyuSettings(Parse(
		R"({"sendReadStories":false,"sendOnlinePackets":true})"), 42);
	const auto global = MigrateAyuSettings(Parse(R"json({
		"useGlobalGhostMode": true,
		"ghostModeSettings": {
			"0": { "sendReadStories": false, "sendOnlinePackets": true },
			"42": { "sendReadStories": true, "sendOnlinePackets": false }
		}
	})json"), 42);
	Require(!legacy.settings.isEmpty() && legacy.settings == global.settings,
		"legacy and global ghost profiles must produce the same settings");
}

TEST_CASE("AyuSettingsImportSkipsMissingProfileAndRejectsInvalidValues") {
	const auto foreign = MigrateAyuSettings(Parse(R"json({
		"useGlobalGhostMode": false,
		"ghostModeSettings": {
			"0": { "sendReadStories": false },
			"43": { "sendOnlinePackets": false }
		},
		"showMessageSeconds": true
	})json"), 42);
	Require(!foreign.settings.isEmpty()
		&& Parse(foreign.settings).value("account").toObject().isEmpty()
		&& foreign.skippedKeys == QStringList{ u"ghostModeSettings"_q },
		"absent account profile must leave ghost settings alone");
	for (const auto &invalid : {
			R"({"disableAds":true,"sendReadMessages":"false"})",
			R"({"disableAds":true,"showPeerId":1.5})",
			R"({"disableAds":true,"ghostModeSettings":{"0":[]}})",
			R"({"disableAds":true,"monoFont":"Family\nName"})",
		}) {
		Require(MigrateAyuSettings(Parse(invalid), 42).settings.isEmpty(),
			"malformed supported values must not be partially imported");
	}
	Require(MigrateAyuSettings(Parse(R"({"unrelated":true})"), 42)
		.settings.isEmpty(), "unrelated JSON must not produce an import");
}

} // namespace
