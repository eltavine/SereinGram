// Generated from proto/serein/config/v1/snapshot.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/snapshot.h"

namespace Serein::Snapshot {

bool Read(
		const QJsonValue &json,
		SnapshotConfig &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("background"),
			QLatin1StringView("date"),
			QLatin1StringView("headers"),
			QLatin1StringView("reactions"),
			QLatin1StringView("builtinTheme"),
			QLatin1StringView("simpleReplies"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("background"),
			QLatin1StringView("date"),
			QLatin1StringView("headers"),
			QLatin1StringView("reactions"),
			QLatin1StringView("builtinTheme"),
			QLatin1StringView("simpleReplies"),
		}, error, path)) {
		return false;
	}
	result = SnapshotConfig();
	return true
		&& Codec::ReadField(object, QLatin1StringView("background"), result.background, error, path)
		&& Codec::ReadField(object, QLatin1StringView("date"), result.date, error, path)
		&& Codec::ReadField(object, QLatin1StringView("headers"), result.headers, error, path)
		&& Codec::ReadField(object, QLatin1StringView("reactions"), result.reactions, error, path)
		&& Codec::ReadField(object, QLatin1StringView("builtinTheme"), result.builtinTheme, error, path)
		&& Codec::ReadField(object, QLatin1StringView("simpleReplies"), result.simpleReplies, error, path);
}

QJsonValue Write(const SnapshotConfig &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("background"), value.background);
	Codec::WriteField(object, QLatin1StringView("date"), value.date);
	Codec::WriteField(object, QLatin1StringView("headers"), value.headers);
	Codec::WriteField(object, QLatin1StringView("reactions"), value.reactions);
	Codec::WriteField(object, QLatin1StringView("builtinTheme"), value.builtinTheme);
	Codec::WriteField(object, QLatin1StringView("simpleReplies"), value.simpleReplies);
	return object;
}

bool Validate(
		const SnapshotConfig &value,
		Codec::Error &error,
		const QString &path) {
	return true;
}

std::optional<SnapshotConfig> ParseSnapshotConfig(
		const QByteArray &raw,
		Codec::Error *error) {
	auto ignored = Codec::Error();
	auto &out = error ? *error : ignored;
	auto object = Codec::ParseObject(raw, out);
	if (!object) {
		return std::nullopt;
	} else if (object->value(QLatin1StringView("version")) != QJsonValue(2)) {
		Codec::Fail(out, QString::fromLatin1("version"), QString::fromLatin1("unsupported version"));
		return std::nullopt;
	}
	object->remove(QLatin1StringView("version"));
	auto result = SnapshotConfig();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeSnapshotConfig(const SnapshotConfig &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 2);
	return Codec::Serialize(object);
}

} // namespace Serein::Snapshot
