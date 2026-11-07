#include "serein/features/presets/model/catalog.h"
#include "base/basic_types.h"
#include "serein/core/exchange.h"
#include "serein/tests/full_registry.h"
#include "serein/tests/memory_prefs.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <array>
#include <string>

namespace {

[[nodiscard]] QByteArray ReadFile(const QString &path) {
	auto file = QFile(path);
	return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}

[[nodiscard]] QString PresetsDir() {
	return QString::fromUtf8(SEREIN_PRESETS_DIR);
}

} // namespace

TEST_CASE("PresetCatalog") {
	using Serein::Presets::ParseCatalog;
	const auto parsed = ParseCatalog(R"({"version":1,"presets":["a","b_2"]})");
	Require(parsed && *parsed == std::vector{ u"a"_q, u"b_2"_q },
		"valid catalog rejected");
	for (const auto &invalid : {
			QByteArray(R"({"version":2,"presets":[]})"),
			QByteArray(R"({"version":1,"presets":["a","a"]})"),
			QByteArray(R"({"version":1,"presets":["Upper"]})"),
			QByteArray(R"({"version":1,"presets":["../x"]})"),
			QByteArray(R"({"version":1,"presets":[1]})"),
			QByteArray(R"({"version":1,"presets":[],"extra":true})"),
			QByteArray(R"([])") }) {
		Require(!ParseCatalog(invalid), "malformed catalog accepted");
	}
}

TEST_CASE("Presets") {
	using namespace Serein;
	const auto catalog = Presets::ParseCatalog(
		ReadFile(PresetsDir() + u"/catalog.json"_q));
	Require(catalog && !catalog->empty(), "preset catalog missing");
	const auto registry = Tests::FullRegistry();
	const auto languages = std::array{
		u"serein.strings"_q,
		u"zh-hans.strings"_q,
		u"zh-hant.strings"_q,
	};
	for (const auto &id : *catalog) {
		INFO(("preset " + id.toStdString()));
		auto devicePrefs = Tests::MemoryPrefs();
		auto accountPrefs = Tests::MemoryPrefs();
		auto device = Options(devicePrefs);
		auto account = Options(accountPrefs, Scope::Account);
		const auto plan = Exchange::PlanImport(
			device,
			&account,
			registry,
			ReadFile(PresetsDir() + u"/"_q + id + u".json"_q));
		Require(plan.error.isEmpty()
			&& plan.skippedKeys.isEmpty()
			&& !plan.changes.empty(),
			"preset no longer matches the registered settings");
		for (const auto &language : languages) {
			const auto text = QString::fromUtf8(ReadFile(
				QString::fromUtf8(SEREIN_LANG_SOURCE_DIR)
					+ u"/serein/"_q
					+ language));
			const auto key = u"\"lng_serein_preset_"_q + id;
			Require(text.contains(key + u"\" = "_q)
				&& text.contains(key + u"_about\" = "_q),
				"preset title or description missing");
		}
	}
	const auto files = QDir(PresetsDir()).entryList(
		{ u"*.json"_q },
		QDir::Files);
	Require(files.size() == int(catalog->size()) + 1,
		"preset file missing from the catalog");
}
