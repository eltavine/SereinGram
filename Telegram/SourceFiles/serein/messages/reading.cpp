#include "serein/hooks/messages/reading.h"

#include "serein/hooks/compose/text.h"
#include "serein/messages/chinese.h"
#include "serein/messages/chinese_warmup.h"
#include "logs.h"
#include "settings.h"
#include "ui/text/text_utilities.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

#include <algorithm>
#include <mutex>

namespace Serein::Messages {
namespace {

bool Protected(EntityType type) {
	switch (type) {
	case EntityType::Url:
	case EntityType::CustomUrl:
	case EntityType::Email:
	case EntityType::Hashtag:
	case EntityType::Cashtag:
	case EntityType::Mention:
	case EntityType::MentionName:
	case EntityType::CustomEmoji:
	case EntityType::BotCommand:
	case EntityType::MediaTimestamp:
	case EntityType::Phone:
	case EntityType::BankCard:
	case EntityType::Code:
	case EntityType::Pre:
	case EntityType::FormattedDate:
		return true;
	default:
		return false;
	}
}

[[nodiscard]] QString DictionaryDirectory() {
	const auto directory = cWorkingDir() + u"tdata/serein/opencc-1.4.2"_q;
	if (!QDir().mkpath(directory)) {
		return QString();
	}
	for (const auto name : kChineseDictionaries) {
		const auto target = directory + '/' + QString::fromLatin1(name);
		if (QFile::exists(target)) {
			continue;
		} else if (!QFile::copy(u":/serein/opencc/"_q + QString::fromLatin1(name), target)) {
			return QString();
		}
		QFile::setPermissions(target, QFile::ReadOwner | QFile::WriteOwner);
	}
	return directory;
}

using Converters = std::array<std::unique_ptr<ChineseConverter>, 2>;

[[nodiscard]] Converters &LoadedConverters() {
	static auto result = Converters();
	return result;
}

void LoadConverter(bool traditional) {
	static auto loaded = std::array<std::once_flag, 2>();
	const auto index = traditional ? 1 : 0;
	std::call_once(loaded[index], [&] {
		const auto directory = DictionaryDirectory();
		auto &converter = LoadedConverters()[index];
		converter = directory.isEmpty()
			? nullptr
			: ChineseConverter::Load(directory, traditional);
		if (!converter) {
			LOG(("Serein Chinese conversion: OpenCC could not be loaded."));
		}
	});
}

[[nodiscard]] const ChineseConverter *Converter(bool traditional) {
	LoadConverter(traditional);
	return LoadedConverters()[traditional ? 1 : 0].get();
}

} // namespace

void WarmUpChineseConversion(bool traditional) {
	crl::async([=] {
		LoadConverter(traditional);
	});
}

std::optional<TextWithEntities> ConvertChinese(
		const TextWithEntities &source,
		bool traditional) {
	const auto converter = Converter(traditional);
	if (!converter) {
		return std::nullopt;
	}
	const auto length = int(source.text.size());
	auto protectedPositions = std::vector<bool>(length);
	const auto protect = [&](const EntitiesInText &entities) {
		for (const auto &entity : entities) {
			if (!entity.validForText(length)) {
				return false;
			}
			if (Protected(entity.type())) {
				std::fill(protectedPositions.begin() + entity.offset(),
					protectedPositions.begin() + entity.offset() + entity.length(), true);
			}
		}
		return true;
	};
	if (!protect(source.entities)
		|| !protect(TextUtilities::ParseEntities(source.text,
			TextParseLinks | TextParseMentions | TextParseHashtags
				| TextParseBotCommands).entities)) {
		LOG(("Serein Chinese conversion: invalid text entity range."));
		return std::nullopt;
	}
	auto codeStart = -1;
	auto codeTicks = 0;
	for (auto i = 0; i < length;) {
		if (source.text.at(i) == u'\\' && codeStart < 0) {
			i += std::min(2, length - i);
		} else if (source.text.at(i) != u'`') {
			++i;
		} else {
			const auto start = i;
			while (i < length && source.text.at(i) == u'`') {
				++i;
			}
			if (codeStart < 0) {
				codeStart = start;
				codeTicks = i - start;
			} else if (i - start == codeTicks) {
				std::fill(protectedPositions.begin() + codeStart,
					protectedPositions.begin() + i, true);
				codeStart = -1;
			}
		}
	}
	if (codeStart >= 0) {
		std::fill(protectedPositions.begin() + codeStart,
			protectedPositions.end(), true);
	}
	return ConvertChineseRuns(source, protectedPositions, [&](
			const QString &text) {
		return converter->convert(text);
	});
}

std::optional<TextWithEntities> ProjectReading(
		const TextWithEntities &source,
		bool spacing,
		int chinese) {
	auto result = source;
	if (chinese && !source.text.isEmpty()) {
		const auto converted = ConvertChinese(source, chinese == 2);
		if (!converted) {
			return std::nullopt;
		}
		result = *converted;
	}
	if (spacing) {
		result = Compose::AddChineseLatinSpacing(result);
	}
	return result;
}

const TextWithEntities &ReadingCache::Get(
		const TextWithEntities &source,
		bool spacing,
		int chinese) {
	if (!_valid || _source != source
		|| _spacing != spacing || _chinese != chinese) {
		_source = source;
		_spacing = spacing;
		_chinese = chinese;
		const auto projected = ProjectReading(source, spacing, chinese);
		_display = projected ? *projected : source;
		_valid = true;
	}
	return _display;
}

} // namespace Serein::Messages
