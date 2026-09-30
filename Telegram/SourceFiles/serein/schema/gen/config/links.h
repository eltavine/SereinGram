// Generated from proto/serein/config/v1/links.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Links {

struct LinkRule {
	QString id;
	QString host;
	QString replacementHost;
	std::vector<QString> removeParameters;
	bool enabled = false;

	friend bool operator==(const LinkRule &, const LinkRule &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	LinkRule &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const LinkRule &value);
[[nodiscard]] bool Validate(
	const LinkRule &value,
	Codec::Error &error,
	const QString &path);

struct LinkRules {
	bool confirmAll = false;
	std::vector<LinkRule> rules;

	friend bool operator==(const LinkRules &, const LinkRules &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	LinkRules &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const LinkRules &value);
[[nodiscard]] bool Validate(
	const LinkRules &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidLinkRules(const LinkRules &value);
[[nodiscard]] std::optional<LinkRules> ParseLinkRules(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeLinkRules(const LinkRules &value);

} // namespace Serein::Links
