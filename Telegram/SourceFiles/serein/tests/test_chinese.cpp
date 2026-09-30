#include "serein/messages/chinese.h"

#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestChinese() {
	using namespace Serein::Messages;
	auto source = TextWithEntities{ QString::fromUtf8("ab\u6F22\u5B57cd") };
	source.entities.push_back(EntityInText(EntityType::Bold, 2, 2));
	auto untouched = std::vector<bool>(source.text.size());
	const auto longer = [](const QString &text) -> std::optional<QString> {
		return text + text;
	};
	const auto doubled = ConvertChineseRuns(source, untouched, longer);
	Require(doubled
		&& doubled->text == QString::fromUtf8("ab\u6F22\u5B57\u6F22\u5B57cd")
		&& doubled->entities.size() == 1
		&& doubled->entities[0].offset() == 2
		&& doubled->entities[0].length() == 4,
		"entity offsets not moved with a longer run");
	auto locked = untouched;
	locked[2] = locked[3] = true;
	const auto kept = ConvertChineseRuns(source, locked, longer);
	Require(kept && kept->text == source.text, "protected text converted");
	const auto failing = [](const QString &) -> std::optional<QString> {
		return std::nullopt;
	};
	Require(!ConvertChineseRuns(source, untouched, failing),
		"failed conversion accepted");
	auto broken = source;
	broken.entities[0] = EntityInText(EntityType::Bold, 4, 9);
	Require(!ConvertChineseRuns(broken, untouched, longer),
		"out of range entity accepted");

	auto directory = QTemporaryDir();
	Require(directory.isValid(), "temporary directory");
	Require(!ChineseConverter::Load(directory.path(), true),
		"converter loaded without dictionaries");
	for (const auto name : kChineseDictionaries) {
		const auto file = QString::fromLatin1(name);
		Require(QFile::copy(
			QString::fromUtf8(SEREIN_OPENCC_DICTIONARY_DIR) + '/' + file,
			directory.filePath(file)), "dictionary copy");
	}
	const auto traditional = ChineseConverter::Load(directory.path(), true);
	const auto simplified = ChineseConverter::Load(directory.path(), false);
	Require(traditional && simplified, "OpenCC converters not loaded");
	Require(traditional->convert(QString::fromUtf8("\u5934\u53D1"))
			== QString::fromUtf8("\u982D\u9AEE"),
		"simplified phrase not converted as a phrase");
	Require(simplified->convert(QString::fromUtf8("\u5F8C\u4F86"))
			== QString::fromUtf8("\u540E\u6765"),
		"traditional phrase not simplified");
}
