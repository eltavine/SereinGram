// Generated from proto/serein/config/v1/quick_replies.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/quick_replies.h"

namespace Serein::Compose {

bool Read(
		const QJsonValue &json,
		QuickReplies &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("replies"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("replies"),
		}, error, path)) {
		return false;
	}
	result = QuickReplies();
	return true
		&& Codec::ReadField(object, QLatin1StringView("replies"), result.replies, error, path);
}

QJsonValue Write(const QuickReplies &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("replies"), value.replies);
	return object;
}

bool Validate(
		const QuickReplies &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.replies.size()) >= 2)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("replies")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(qsizetype(value.replies.size()) <= 2)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("replies")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

std::optional<QuickReplies> ParseQuickReplies(
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
	auto result = QuickReplies();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeQuickReplies(const QuickReplies &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Compose
