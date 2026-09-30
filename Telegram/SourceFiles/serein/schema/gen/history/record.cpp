// Generated from proto/serein/history/v1/record.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/history/record.h"

namespace Serein::History {

bool Read(
		const QJsonValue &json,
		RecordKind &result,
		Codec::Error &error,
		const QString &path) {
	if (json.isString()) {
		const auto name = json.toString();
		if (name == QLatin1StringView("RECORD_KIND_UNSPECIFIED")) {
			result = RecordKind::Unspecified;
			return true;
		}
		if (name == QLatin1StringView("RECORD_KIND_DELETED")) {
			result = RecordKind::Deleted;
			return true;
		}
		if (name == QLatin1StringView("RECORD_KIND_EDITED")) {
			result = RecordKind::Edited;
			return true;
		}
	} else if (json.isDouble()) {
		switch (json.toInt(-1)) {
		case 0: result = RecordKind::Unspecified; return true;
		case 1: result = RecordKind::Deleted; return true;
		case 2: result = RecordKind::Edited; return true;
		}
	}
	return Codec::FailExpected(error, path, "a RecordKind value");
}

QJsonValue Write(RecordKind value) {
	switch (value) {
	case RecordKind::Unspecified: return QString::fromLatin1("RECORD_KIND_UNSPECIFIED");
	case RecordKind::Deleted: return QString::fromLatin1("RECORD_KIND_DELETED");
	case RecordKind::Edited: return QString::fromLatin1("RECORD_KIND_EDITED");
	}
	return QString::fromLatin1("RECORD_KIND_UNSPECIFIED");
}

bool Read(
		const QJsonValue &json,
		TextEntity &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("type"),
			QLatin1StringView("offset"),
			QLatin1StringView("length"),
			QLatin1StringView("data"),
		}, error, path)) {
		return false;
	}
	result = TextEntity();
	return true
		&& Codec::ReadField(object, QLatin1StringView("type"), result.type, error, path)
		&& Codec::ReadField(object, QLatin1StringView("offset"), result.offset, error, path)
		&& Codec::ReadField(object, QLatin1StringView("length"), result.length, error, path)
		&& Codec::ReadField(object, QLatin1StringView("data"), result.data, error, path);
}

QJsonValue Write(const TextEntity &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("type"), value.type);
	Codec::WriteField(object, QLatin1StringView("offset"), value.offset);
	Codec::WriteField(object, QLatin1StringView("length"), value.length);
	Codec::WriteField(object, QLatin1StringView("data"), value.data);
	return object;
}

bool Validate(
		const TextEntity &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Matches(value.type, QString::fromUtf8("^[a-z_]{1,32}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("type")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.offset >= 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("offset")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.length > 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("length")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		Record &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("kind"),
			QLatin1StringView("peerId"),
			QLatin1StringView("messageId"),
			QLatin1StringView("topicRootId"),
			QLatin1StringView("revision"),
			QLatin1StringView("date"),
			QLatin1StringView("recordedAt"),
			QLatin1StringView("fromPeerId"),
			QLatin1StringView("text"),
			QLatin1StringView("entities"),
			QLatin1StringView("apiLayer"),
			QLatin1StringView("tlMessage"),
			QLatin1StringView("mediaSummary"),
			QLatin1StringView("localPath"),
		}, error, path)) {
		return false;
	}
	result = Record();
	return true
		&& Codec::ReadField(object, QLatin1StringView("kind"), result.kind, error, path)
		&& Codec::ReadField(object, QLatin1StringView("peerId"), result.peerId, error, path)
		&& Codec::ReadField(object, QLatin1StringView("messageId"), result.messageId, error, path)
		&& Codec::ReadField(object, QLatin1StringView("topicRootId"), result.topicRootId, error, path)
		&& Codec::ReadField(object, QLatin1StringView("revision"), result.revision, error, path)
		&& Codec::ReadField(object, QLatin1StringView("date"), result.date, error, path)
		&& Codec::ReadField(object, QLatin1StringView("recordedAt"), result.recordedAt, error, path)
		&& Codec::ReadField(object, QLatin1StringView("fromPeerId"), result.fromPeerId, error, path)
		&& Codec::ReadField(object, QLatin1StringView("text"), result.text, error, path)
		&& Codec::ReadField(object, QLatin1StringView("entities"), result.entities, error, path)
		&& Codec::ReadField(object, QLatin1StringView("apiLayer"), result.apiLayer, error, path)
		&& Codec::ReadField(object, QLatin1StringView("tlMessage"), result.tlMessage, error, path)
		&& Codec::ReadField(object, QLatin1StringView("mediaSummary"), result.mediaSummary, error, path)
		&& Codec::ReadField(object, QLatin1StringView("localPath"), result.localPath, error, path);
}

QJsonValue Write(const Record &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("kind"), value.kind);
	Codec::WriteField(object, QLatin1StringView("peerId"), value.peerId);
	Codec::WriteField(object, QLatin1StringView("messageId"), value.messageId);
	Codec::WriteField(object, QLatin1StringView("topicRootId"), value.topicRootId);
	Codec::WriteField(object, QLatin1StringView("revision"), value.revision);
	Codec::WriteField(object, QLatin1StringView("date"), value.date);
	Codec::WriteField(object, QLatin1StringView("recordedAt"), value.recordedAt);
	Codec::WriteField(object, QLatin1StringView("fromPeerId"), value.fromPeerId);
	Codec::WriteField(object, QLatin1StringView("text"), value.text);
	Codec::WriteField(object, QLatin1StringView("entities"), value.entities);
	Codec::WriteField(object, QLatin1StringView("apiLayer"), value.apiLayer);
	Codec::WriteField(object, QLatin1StringView("tlMessage"), value.tlMessage);
	Codec::WriteField(object, QLatin1StringView("mediaSummary"), value.mediaSummary);
	Codec::WriteField(object, QLatin1StringView("localPath"), value.localPath);
	return object;
}

bool Validate(
		const Record &value,
		Codec::Error &error,
		const QString &path) {
	if (!(int(value.kind) != 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("kind")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.peerId != 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("peerId")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.messageId > 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("messageId")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.revision >= 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("revision")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(qsizetype(value.entities.size()) <= 10000)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("entities")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.entities.size()); ++i) {
		const auto &item = value.entities[i];
		if (!Validate(item, error, Codec::Item(Codec::Child(path, QLatin1StringView("entities")), i))) {
			return false;
		}
	}
	if (!(value.apiLayer >= 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("apiLayer")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.localPath.toUcs4().size() <= 4096)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("localPath")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

std::optional<Record> ParseRecord(
		const QByteArray &raw,
		Codec::Error *error) {
	auto ignored = Codec::Error();
	auto &out = error ? *error : ignored;
	auto object = Codec::ParseObject(raw, out);
	if (!object) {
		return std::nullopt;
	} else if (object->value(QLatin1StringView("version")) != QJsonValue(1)) {
		Codec::Fail(out, QString::fromLatin1("version"), QString::fromLatin1("unsupported version"));
		return std::nullopt;
	}
	object->remove(QLatin1StringView("version"));
	auto result = Record();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeRecord(const Record &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::History
