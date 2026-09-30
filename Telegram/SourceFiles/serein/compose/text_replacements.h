#pragma once

#include "serein/schema/gen/config/text_replacements.h"

#include <optional>

namespace Serein::Compose {

[[nodiscard]] std::optional<TextReplacements> ReadTextReplacements(
	const QByteArray &raw);
[[nodiscard]] QByteArray WriteTextReplacements(const TextReplacements &value);
[[nodiscard]] QString FormatReplacementLines(const TextReplacements &value);
[[nodiscard]] std::optional<TextReplacements> ParseReplacementLines(
	const QString &text);

} // namespace Serein::Compose
