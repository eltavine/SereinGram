// Generated from proto/serein/config/v1/history_exclusions.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::HistoryFeature {

struct HistoryExclusions {
	std::vector<QString> peers;

	friend bool operator==(const HistoryExclusions &, const HistoryExclusions &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	HistoryExclusions &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const HistoryExclusions &value);
[[nodiscard]] bool Validate(
	const HistoryExclusions &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] std::optional<HistoryExclusions> ParseHistoryExclusions(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeHistoryExclusions(const HistoryExclusions &value);

} // namespace Serein::HistoryFeature
