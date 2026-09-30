// Generated from proto/serein/config/v1/text_replacements.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Compose {

struct TextReplacement {
	QString from;
	QString to;

	friend bool operator==(const TextReplacement &, const TextReplacement &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	TextReplacement &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const TextReplacement &value);
[[nodiscard]] bool Validate(
	const TextReplacement &value,
	Codec::Error &error,
	const QString &path);

struct TextReplacements {
	std::vector<TextReplacement> rules;

	friend bool operator==(const TextReplacements &, const TextReplacements &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	TextReplacements &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const TextReplacements &value);
[[nodiscard]] bool Validate(
	const TextReplacements &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidTextReplacements(const TextReplacements &value);
[[nodiscard]] std::optional<TextReplacements> ParseTextReplacements(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeTextReplacements(const TextReplacements &value);

} // namespace Serein::Compose
