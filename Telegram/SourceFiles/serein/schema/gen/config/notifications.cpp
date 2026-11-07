// Generated from proto/serein/config/v1/notifications.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/notifications.h"

namespace Serein::Notifications {

bool Read(
		const QJsonValue &json,
		QuietHours &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("enabled"),
			QLatin1StringView("startMinute"),
			QLatin1StringView("endMinute"),
			QLatin1StringView("weekdays"),
			QLatin1StringView("allowContacts"),
			QLatin1StringView("allowPinned"),
			QLatin1StringView("allowMentions"),
			QLatin1StringView("allowKeywords"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("enabled"),
			QLatin1StringView("startMinute"),
			QLatin1StringView("endMinute"),
			QLatin1StringView("weekdays"),
			QLatin1StringView("allowContacts"),
			QLatin1StringView("allowPinned"),
			QLatin1StringView("allowMentions"),
			QLatin1StringView("allowKeywords"),
		}, error, path)) {
		return false;
	}
	result = QuietHours();
	return true
		&& Codec::ReadField(object, QLatin1StringView("enabled"), result.enabled, error, path)
		&& Codec::ReadField(object, QLatin1StringView("startMinute"), result.startMinute, error, path)
		&& Codec::ReadField(object, QLatin1StringView("endMinute"), result.endMinute, error, path)
		&& Codec::ReadField(object, QLatin1StringView("weekdays"), result.weekdays, error, path)
		&& Codec::ReadField(object, QLatin1StringView("allowContacts"), result.allowContacts, error, path)
		&& Codec::ReadField(object, QLatin1StringView("allowPinned"), result.allowPinned, error, path)
		&& Codec::ReadField(object, QLatin1StringView("allowMentions"), result.allowMentions, error, path)
		&& Codec::ReadField(object, QLatin1StringView("allowKeywords"), result.allowKeywords, error, path);
}

QJsonValue Write(const QuietHours &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("enabled"), value.enabled);
	Codec::WriteField(object, QLatin1StringView("startMinute"), value.startMinute);
	Codec::WriteField(object, QLatin1StringView("endMinute"), value.endMinute);
	Codec::WriteField(object, QLatin1StringView("weekdays"), value.weekdays);
	Codec::WriteField(object, QLatin1StringView("allowContacts"), value.allowContacts);
	Codec::WriteField(object, QLatin1StringView("allowPinned"), value.allowPinned);
	Codec::WriteField(object, QLatin1StringView("allowMentions"), value.allowMentions);
	Codec::WriteField(object, QLatin1StringView("allowKeywords"), value.allowKeywords);
	return object;
}

bool Validate(
		const QuietHours &value,
		Codec::Error &error,
		const QString &path) {
	if (!(value.startMinute <= 1439 && value.startMinute >= 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("startMinute")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.endMinute <= 1439 && value.endMinute >= 0)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("endMinute")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(qsizetype(value.weekdays.size()) <= 7)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("weekdays")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

std::optional<QuietHours> ParseQuietHours(
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
	auto result = QuietHours();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidQuietHours(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeQuietHours(const QuietHours &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

bool Read(
		const QJsonValue &json,
		KeywordRule &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("pattern"),
			QLatin1StringView("regex"),
			QLatin1StringView("caseSensitive"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("pattern"),
			QLatin1StringView("regex"),
			QLatin1StringView("caseSensitive"),
		}, error, path)) {
		return false;
	}
	result = KeywordRule();
	return true
		&& Codec::ReadField(object, QLatin1StringView("pattern"), result.pattern, error, path)
		&& Codec::ReadField(object, QLatin1StringView("regex"), result.regex, error, path)
		&& Codec::ReadField(object, QLatin1StringView("caseSensitive"), result.caseSensitive, error, path);
}

QJsonValue Write(const KeywordRule &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("pattern"), value.pattern);
	Codec::WriteField(object, QLatin1StringView("regex"), value.regex);
	Codec::WriteField(object, QLatin1StringView("caseSensitive"), value.caseSensitive);
	return object;
}

bool Validate(
		const KeywordRule &value,
		Codec::Error &error,
		const QString &path) {
	if (!(value.pattern.toUcs4().size() >= 1 && value.pattern.toUcs4().size() <= 256)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("pattern")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		KeywordAlerts &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("enabled"),
			QLatin1StringView("rules"),
			QLatin1StringView("includeChannels"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("enabled"),
			QLatin1StringView("rules"),
			QLatin1StringView("includeChannels"),
		}, error, path)) {
		return false;
	}
	result = KeywordAlerts();
	return true
		&& Codec::ReadField(object, QLatin1StringView("enabled"), result.enabled, error, path)
		&& Codec::ReadField(object, QLatin1StringView("rules"), result.rules, error, path)
		&& Codec::ReadField(object, QLatin1StringView("includeChannels"), result.includeChannels, error, path);
}

QJsonValue Write(const KeywordAlerts &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("enabled"), value.enabled);
	Codec::WriteField(object, QLatin1StringView("rules"), value.rules);
	Codec::WriteField(object, QLatin1StringView("includeChannels"), value.includeChannels);
	return object;
}

bool Validate(
		const KeywordAlerts &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.rules.size()) <= 50)) {
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

std::optional<KeywordAlerts> ParseKeywordAlerts(
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
	auto result = KeywordAlerts();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidKeywordAlerts(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeKeywordAlerts(const KeywordAlerts &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Notifications
