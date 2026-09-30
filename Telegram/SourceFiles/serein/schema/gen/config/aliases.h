// Generated from proto/serein/config/v1/aliases.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Privacy {

struct PeerAliasesConfig {
	std::map<QString, QString> names;

	friend bool operator==(const PeerAliasesConfig &, const PeerAliasesConfig &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	PeerAliasesConfig &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const PeerAliasesConfig &value);
[[nodiscard]] bool Validate(
	const PeerAliasesConfig &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidPeerAliasesConfig(const PeerAliasesConfig &value);
[[nodiscard]] std::optional<PeerAliasesConfig> ParsePeerAliasesConfig(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializePeerAliasesConfig(const PeerAliasesConfig &value);

} // namespace Serein::Privacy
