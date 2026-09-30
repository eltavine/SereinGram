#pragma once

#include "base/assertion.h"
#include "base/basic_types.h"
#include "ui/text/text_entity.h"

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace opencc {
class SimpleConverter;
} // namespace opencc

namespace Serein::Messages {

inline constexpr auto kChineseDictionaries = std::array{
	"STCharacters.txt",
	"STPhrases.txt",
	"TSCharacters.txt",
	"TSPhrases.txt",
};

using ChineseConvert = std::function<std::optional<QString>(
	const QString &text)>;

// Runs split at entity edges keep offsets exact when phrases change length.
[[nodiscard]] std::optional<TextWithEntities> ConvertChineseRuns(
	const TextWithEntities &source,
	const std::vector<bool> &protectedPositions,
	const ChineseConvert &convert);

class ChineseConverter final {
public:
	[[nodiscard]] static std::unique_ptr<ChineseConverter> Load(
		const QString &directory,
		bool traditional);
	~ChineseConverter();

	[[nodiscard]] std::optional<QString> convert(const QString &text) const;

private:
	explicit ChineseConverter(
		std::unique_ptr<opencc::SimpleConverter> converter);

	std::unique_ptr<opencc::SimpleConverter> _converter;

};

} // namespace Serein::Messages
