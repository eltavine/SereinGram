// Generated from proto/serein/config/v1/link_inline_bots.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/link_inline_bots.h"

namespace Serein::Compose {

bool Read(
		const QJsonValue &json,
		LinkInlineBot &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("bot"),
			QLatin1StringView("pattern"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("bot"),
			QLatin1StringView("pattern"),
		}, error, path)) {
		return false;
	}
	result = LinkInlineBot();
	return true
		&& Codec::ReadField(object, QLatin1StringView("bot"), result.bot, error, path)
		&& Codec::ReadField(object, QLatin1StringView("pattern"), result.pattern, error, path);
}

QJsonValue Write(const LinkInlineBot &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("bot"), value.bot);
	Codec::WriteField(object, QLatin1StringView("pattern"), value.pattern);
	return object;
}

bool Validate(
		const LinkInlineBot &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Matches(value.bot, QString::fromUtf8("^[A-Za-z][A-Za-z0-9_]{2,31}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("bot")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.pattern.toUcs4().size() >= 1 && value.pattern.toUcs4().size() <= 256)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("pattern")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		LinkInlineBots &result,
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
	result = LinkInlineBots();
	return true
		&& Codec::ReadField(object, QLatin1StringView("rules"), result.rules, error, path);
}

QJsonValue Write(const LinkInlineBots &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("rules"), value.rules);
	return object;
}

bool Validate(
		const LinkInlineBots &value,
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

std::optional<LinkInlineBots> ParseLinkInlineBots(
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
	auto result = LinkInlineBots();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidLinkInlineBots(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeLinkInlineBots(const LinkInlineBots &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Compose
