// Generated from proto/serein/history/v1/record.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::History {

enum class RecordKind {
	Unspecified = 0,
	Deleted = 1,
	Edited = 2,
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	RecordKind &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(RecordKind value);

struct TextEntity {
	QString type;
	int offset = 0;
	int length = 0;
	QString data;

	friend bool operator==(const TextEntity &, const TextEntity &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	TextEntity &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const TextEntity &value);
[[nodiscard]] bool Validate(
	const TextEntity &value,
	Codec::Error &error,
	const QString &path);

struct Record {
	RecordKind kind = RecordKind::Unspecified;
	qint64 peerId = 0;
	qint64 messageId = 0;
	qint64 topicRootId = 0;
	int revision = 0;
	qint64 date = 0;
	qint64 recordedAt = 0;
	qint64 fromPeerId = 0;
	QString text;
	std::vector<TextEntity> entities;
	int apiLayer = 0;
	QByteArray tlMessage;
	QString mediaSummary;
	QString localPath;

	friend bool operator==(const Record &, const Record &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	Record &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const Record &value);
[[nodiscard]] bool Validate(
	const Record &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] std::optional<Record> ParseRecord(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeRecord(const Record &value);

} // namespace Serein::History
