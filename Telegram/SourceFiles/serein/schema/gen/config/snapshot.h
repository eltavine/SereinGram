// Generated from proto/serein/config/v1/snapshot.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Snapshot {

struct SnapshotConfig {
	bool background = false;
	bool date = false;
	bool headers = false;
	bool reactions = false;
	bool builtinTheme = false;
	bool simpleReplies = false;

	friend bool operator==(const SnapshotConfig &, const SnapshotConfig &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	SnapshotConfig &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const SnapshotConfig &value);
[[nodiscard]] bool Validate(
	const SnapshotConfig &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] std::optional<SnapshotConfig> ParseSnapshotConfig(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeSnapshotConfig(const SnapshotConfig &value);

} // namespace Serein::Snapshot
