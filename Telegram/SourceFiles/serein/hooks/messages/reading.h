#pragma once

#include "ui/text/text_entity.h"

#include <optional>

namespace Serein::Messages {

[[nodiscard]] constexpr bool ChineseConversionAvailable() {
	return true;
}

[[nodiscard]] std::optional<TextWithEntities> ConvertChinese(
	const TextWithEntities &source,
	bool traditional);
[[nodiscard]] std::optional<TextWithEntities> ProjectReading(
	const TextWithEntities &source,
	bool spacing,
	int chinese);

class ReadingCache final {
public:
	[[nodiscard]] const TextWithEntities &Get(
		const TextWithEntities &source,
		bool spacing,
		int chinese);

private:
	TextWithEntities _source;
	TextWithEntities _display;
	bool _spacing = false;
	int _chinese = 0;
	bool _valid = false;
};

} // namespace Serein::Messages
