#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>

void TestOptions();
void TestSpacing();
void TestServices();
void TestTranslationProtocols();
void TestUpdates();
void TestStickers();
void TestRecentChats();
void TestMentionQuery();
void TestRecorders();
void TestReadingPositions();
void TestLocalPins();
void TestSummary();
void TestTextReplacements();
void TestProxySubscription();
void TestVpnRules();
void TestProxyOrder();
void TestProxyNotes();
void TestHiddenMessages();
void TestBatches();
void TestDownloadNames();
void TestShownOrder();
void TestGhostExceptions();
void TestPersianCalendar();
void TestNeutralDefaults();
void TestChinese();
void TestAliases();
void TestStickerCatalog();
void TestSnapshotConfig();
void TestRegistrationDate();
void TestFilters();
void TestLinks();
void TestCodec();
void TestGhost();
void TestCipher();
void TestCredentials();
void TestHistoryStore();
void TestHistoryRecorder();
void TestCachedMedia();

namespace {

using Strings = std::map<std::string, std::string>;

[[nodiscard]] std::optional<std::pair<std::string, std::string>> ParseEntry(
		const std::string &line) {
	auto i = std::size_t();
	const auto skip = [&] {
		while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
			++i;
		}
	};
	const auto quoted = [&](bool escapes) -> std::optional<std::string> {
		if (i >= line.size() || line[i] != '"') {
			return std::nullopt;
		}
		const auto start = ++i;
		while (i < line.size() && line[i] != '"') {
			if (escapes && line[i] == '\\') {
				if (i + 1 >= line.size()) {
					return std::nullopt;
				}
				++i;
			}
			++i;
		}
		if (i >= line.size()) {
			return std::nullopt;
		}
		return line.substr(start, i++ - start);
	};
	skip();
	const auto key = quoted(false);
	if (!key || key->empty()) {
		return std::nullopt;
	}
	skip();
	if (i >= line.size() || line[i] != '=') {
		return std::nullopt;
	}
	++i;
	skip();
	const auto value = quoted(true);
	if (!value || i >= line.size() || line[i] != ';') {
		return std::nullopt;
	}
	++i;
	skip();
	if (i != line.size()) {
		return std::nullopt;
	}
	return std::pair(*key, *value);
}

[[nodiscard]] Strings ReadStrings(const std::string &path, bool strict) {
	auto input = std::ifstream(path);
	if (!input) {
		throw std::runtime_error("Cannot open " + path);
	}

	auto result = Strings();
	auto line = std::string();
	auto number = 0;
	while (std::getline(input, line)) {
		++number;
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		const auto first = line.find_first_not_of(" \t");
		if (first == std::string::npos || line[first] != '"') {
			continue;
		}
		const auto entry = ParseEntry(line);
		if (!entry) {
			if (strict) {
				throw std::runtime_error(path + ":" + std::to_string(number)
					+ ": invalid string entry");
			}
			continue;
		}
		const auto &[key, value] = *entry;
		if (!result.emplace(key, value).second) {
			throw std::runtime_error(path + ":" + std::to_string(number)
				+ ": duplicate key " + key);
		}
	}
	return result;
}

[[nodiscard]] std::multiset<std::string> Placeholders(
		const std::string &value) {
	const auto pattern = std::regex(R"(\{[a-zA-Z_][a-zA-Z_0-9]*\})");
	auto result = std::multiset<std::string>();
	for (auto i = std::sregex_iterator(value.begin(), value.end(), pattern);
		i != std::sregex_iterator(); ++i) {
		result.insert(i->str());
	}
	return result;
}

void CheckTranslation(
		const Strings &english,
		const std::string &path) {
	const auto translated = ReadStrings(path, true);
	if (translated.size() != english.size()) {
		throw std::runtime_error(path + ": key count differs from English");
	}
	for (const auto &[key, value] : english) {
		const auto found = translated.find(key);
		if (found == translated.end()) {
			throw std::runtime_error(path + ": missing key " + key);
		}
		if (Placeholders(value) != Placeholders(found->second)) {
			throw std::runtime_error(path + ": placeholder mismatch for " + key);
		}
	}
}

void CheckPartialTranslation(
		const Strings &english,
		const std::string &path) {
	for (const auto &[key, value] : ReadStrings(path, true)) {
		const auto found = english.find(key);
		if (found == english.end()) {
			throw std::runtime_error(path + ": unknown key " + key);
		}
		if (Placeholders(found->second) != Placeholders(value)) {
			throw std::runtime_error(path + ": placeholder mismatch for " + key);
		}
	}
}

} // namespace

int main() {
	try {
		TestOptions();
		TestSpacing();
		TestServices();
		TestTranslationProtocols();
		TestUpdates();
		TestStickers();
		TestRecentChats();
		TestMentionQuery();
		TestRecorders();
		TestReadingPositions();
		TestLocalPins();
		TestSummary();
		TestTextReplacements();
		TestProxySubscription();
		TestVpnRules();
		TestProxyOrder();
		TestProxyNotes();
		TestHiddenMessages();
		TestBatches();
		TestDownloadNames();
		TestShownOrder();
		TestGhostExceptions();
		TestPersianCalendar();
		TestNeutralDefaults();
		TestChinese();
		TestAliases();
		TestStickerCatalog();
		TestSnapshotConfig();
		TestRegistrationDate();
		TestFilters();
		TestLinks();
		TestCodec();
		TestGhost();
		TestCipher();
		TestCredentials();
#ifdef SEREIN_HAVE_QT_SQL
		auto argc = 1;
		char name[] = "test_serein";
		char *argv[] = { name, nullptr };
		QCoreApplication application(argc, argv);
		TestHistoryStore();
		TestHistoryRecorder();
		TestCachedMedia();
#endif // SEREIN_HAVE_QT_SQL
		const auto root = std::string(SEREIN_LANG_SOURCE_DIR);
		const auto upstream = ReadStrings(root + "/lang.strings", false);
		const auto english = ReadStrings(root + "/serein/serein.strings", true);
		if (english.empty()) {
			throw std::runtime_error("Serein English strings are empty");
		}
		for (const auto &entry : english) {
			const auto &key = entry.first;
			if (!key.starts_with("lng_serein_")) {
				throw std::runtime_error("Invalid Serein key prefix: " + key);
			}
			if (key.ends_with("#one") || key.ends_with("#other")) {
				throw std::runtime_error("Serein plural key is unsupported: " + key);
			}
			if (upstream.contains(key)) {
				throw std::runtime_error("Key collides with upstream: " + key);
			}
		}
		for (const auto &locale : { "zh-hans", "zh-hant" }) {
			const auto path = root + "/serein/" + locale + ".strings";
			CheckTranslation(english, path);
		}
		const auto directory = QDir(QString::fromStdString(root + "/serein"));
		for (const auto &entry : directory.entryList(
				{ QString::fromLatin1("*.strings") },
				QDir::Files)) {
			const auto name = entry.toStdString();
			if (name != "serein.strings" && !name.starts_with("zh-")) {
				CheckPartialTranslation(english, root + "/serein/" + name);
			}
		}
		std::cout << "PASS: Serein strings (" << english.size()
			<< " English keys)" << std::endl;
		return 0;
	} catch (const std::exception &error) {
		std::cerr << "FAIL: " << error.what() << std::endl;
		return 1;
	}
}
