// Generated from proto/serein/config/v1/link_inline_bots.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Compose {

struct LinkInlineBot {
	QString bot;
	QString pattern;

	friend bool operator==(const LinkInlineBot &, const LinkInlineBot &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	LinkInlineBot &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const LinkInlineBot &value);
[[nodiscard]] bool Validate(
	const LinkInlineBot &value,
	Codec::Error &error,
	const QString &path);

struct LinkInlineBots {
	std::vector<LinkInlineBot> rules;

	friend bool operator==(const LinkInlineBots &, const LinkInlineBots &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	LinkInlineBots &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const LinkInlineBots &value);
[[nodiscard]] bool Validate(
	const LinkInlineBots &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidLinkInlineBots(const LinkInlineBots &value);
[[nodiscard]] std::optional<LinkInlineBots> ParseLinkInlineBots(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeLinkInlineBots(const LinkInlineBots &value);

} // namespace Serein::Compose
