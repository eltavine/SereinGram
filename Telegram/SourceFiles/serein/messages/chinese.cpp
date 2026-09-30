#include "serein/messages/chinese.h"

#include "SimpleConverter.hpp"

#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>

#include <algorithm>
#include <exception>

namespace Serein::Messages {
namespace {

[[nodiscard]] QJsonObject TextDictionary(const QString &file) {
	return {
		{ u"type"_q, u"text"_q },
		{ u"file"_q, file },
	};
}

[[nodiscard]] QByteArray ConfigJson(bool traditional) {
	const auto phrases = traditional
		? u"STPhrases.txt"_q
		: u"TSPhrases.txt"_q;
	const auto characters = traditional
		? u"STCharacters.txt"_q
		: u"TSCharacters.txt"_q;
	return QJsonDocument(QJsonObject{
		{ u"name"_q, u"SereinGram"_q },
		{ u"segmentation"_q, QJsonObject{
			{ u"type"_q, u"mmseg"_q },
			{ u"dict"_q, TextDictionary(phrases) },
		} },
		{ u"conversion_chain"_q, QJsonArray{ QJsonObject{
			{ u"dict"_q, QJsonObject{
				{ u"type"_q, u"group"_q },
				{ u"dicts"_q, QJsonArray{
					TextDictionary(phrases),
					TextDictionary(characters),
				} },
			} },
		} } },
	}).toJson(QJsonDocument::Compact);
}

[[nodiscard]] bool ContainsHan(QStringView text) {
	for (auto i = 0; i < text.size(); ++i) {
		const auto first = text[i];
		const auto pair = first.isHighSurrogate()
			&& i + 1 < text.size()
			&& text[i + 1].isLowSurrogate();
		const auto value = pair
			? QChar::surrogateToUcs4(first, text[i + 1])
			: char32_t(first.unicode());
		if (QChar::script(value) == QChar::Script_Han) {
			return true;
		}
		i += pair ? 1 : 0;
	}
	return false;
}

} // namespace

std::optional<TextWithEntities> ConvertChineseRuns(
		const TextWithEntities &source,
		const std::vector<bool> &protectedPositions,
		const ChineseConvert &convert) {
	const auto length = int(source.text.size());
	if (int(protectedPositions.size()) != length) {
		return std::nullopt;
	}
	auto cuts = std::vector<int>{ 0, length };
	for (const auto &entity : source.entities) {
		const auto from = entity.offset();
		const auto till = from + entity.length();
		if (from < 0 || entity.length() < 0 || till > length) {
			return std::nullopt;
		}
		cuts.push_back(from);
		cuts.push_back(till);
	}
	for (auto i = 1; i < length; ++i) {
		if (protectedPositions[i] != protectedPositions[i - 1]) {
			cuts.push_back(i);
		}
	}
	std::sort(cuts.begin(), cuts.end());
	cuts.erase(std::unique(cuts.begin(), cuts.end()), cuts.end());

	auto result = TextWithEntities();
	auto offsets = std::vector<int>(length + 1);
	for (auto k = 0; k + 1 < int(cuts.size()); ++k) {
		const auto from = cuts[k];
		const auto till = cuts[k + 1];
		offsets[from] = int(result.text.size());
		const auto run = QStringView(source.text).mid(from, till - from);
		if (protectedPositions[from] || !ContainsHan(run)) {
			result.text += run;
		} else if (const auto converted = convert(run.toString())) {
			result.text += *converted;
		} else {
			return std::nullopt;
		}
	}
	offsets[length] = int(result.text.size());
	for (const auto &entity : source.entities) {
		const auto start = offsets[entity.offset()];
		const auto end = offsets[entity.offset() + entity.length()];
		auto mapped = entity;
		mapped.shiftRight(start - entity.offset());
		mapped.shrinkFromRight(entity.length() - (end - start));
		result.entities.push_back(mapped);
	}
	return result;
}

std::unique_ptr<ChineseConverter> ChineseConverter::Load(
		const QString &directory,
		bool traditional) {
	const auto path = QDir(directory).filePath(traditional
		? u"serein_s2t.json"_q
		: u"serein_t2s.json"_q);
	auto file = QSaveFile(path);
	if (!file.open(QIODevice::WriteOnly)
		|| file.write(ConfigJson(traditional)) < 0
		|| !file.commit()) {
		return nullptr;
	}
	try {
		auto converter = std::make_unique<opencc::SimpleConverter>(
			path.toStdString());
		return std::unique_ptr<ChineseConverter>(
			new ChineseConverter(std::move(converter)));
	} catch (const std::exception &) {
		return nullptr;
	}
}

ChineseConverter::ChineseConverter(
	std::unique_ptr<opencc::SimpleConverter> converter)
: _converter(std::move(converter)) {
}

ChineseConverter::~ChineseConverter() = default;

std::optional<QString> ChineseConverter::convert(const QString &text) const {
	try {
		return QString::fromStdString(_converter->Convert(text.toStdString()));
	} catch (const std::exception &) {
		return std::nullopt;
	}
}

} // namespace Serein::Messages
