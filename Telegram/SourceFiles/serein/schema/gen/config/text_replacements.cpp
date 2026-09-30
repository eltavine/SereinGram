// Generated from proto/serein/config/v1/text_replacements.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/text_replacements.h"

namespace Serein::Compose {

bool Read(
		const QJsonValue &json,
		TextReplacement &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("from"),
			QLatin1StringView("to"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("from"),
			QLatin1StringView("to"),
		}, error, path)) {
		return false;
	}
	result = TextReplacement();
	return true
		&& Codec::ReadField(object, QLatin1StringView("from"), result.from, error, path)
		&& Codec::ReadField(object, QLatin1StringView("to"), result.to, error, path);
}

QJsonValue Write(const TextReplacement &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("from"), value.from);
	Codec::WriteField(object, QLatin1StringView("to"), value.to);
	return object;
}

bool Validate(
		const TextReplacement &value,
		Codec::Error &error,
		const QString &path) {
	if (!(value.from.toUcs4().size() >= 1 && value.from.toUcs4().size() <= 32)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("from")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.to.toUcs4().size() <= 256)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("to")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		TextReplacements &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("rules"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("rules"),
		}, error, path)) {
		return false;
	}
	result = TextReplacements();
	return true
		&& Codec::ReadField(object, QLatin1StringView("rules"), result.rules, error, path);
}

QJsonValue Write(const TextReplacements &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("rules"), value.rules);
	return object;
}

bool Validate(
		const TextReplacements &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.rules.size()) <= 100)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("rules")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.rules.size()); ++i) {
		const auto &item = value.rules[i];
		if (!Validate(item, error, Codec::Item(Codec::Child(path, QLatin1StringView("rules")), i))) {
			return false;
		}
	}
	return true;
}

std::optional<TextReplacements> ParseTextReplacements(
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
	auto result = TextReplacements();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidTextReplacements(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeTextReplacements(const TextReplacements &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Compose
