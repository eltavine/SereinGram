// Generated from proto/serein/config/v1/links.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/links.h"

namespace Serein::Links {

bool Read(
		const QJsonValue &json,
		LinkRule &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("id"),
			QLatin1StringView("host"),
			QLatin1StringView("replacementHost"),
			QLatin1StringView("removeParameters"),
			QLatin1StringView("enabled"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("id"),
			QLatin1StringView("host"),
			QLatin1StringView("replacementHost"),
			QLatin1StringView("removeParameters"),
			QLatin1StringView("enabled"),
		}, error, path)) {
		return false;
	}
	result = LinkRule();
	return true
		&& Codec::ReadField(object, QLatin1StringView("id"), result.id, error, path)
		&& Codec::ReadField(object, QLatin1StringView("host"), result.host, error, path)
		&& Codec::ReadField(object, QLatin1StringView("replacementHost"), result.replacementHost, error, path)
		&& Codec::ReadField(object, QLatin1StringView("removeParameters"), result.removeParameters, error, path)
		&& Codec::ReadField(object, QLatin1StringView("enabled"), result.enabled, error, path);
}

QJsonValue Write(const LinkRule &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("id"), value.id);
	Codec::WriteField(object, QLatin1StringView("host"), value.host);
	Codec::WriteField(object, QLatin1StringView("replacementHost"), value.replacementHost);
	Codec::WriteField(object, QLatin1StringView("removeParameters"), value.removeParameters);
	Codec::WriteField(object, QLatin1StringView("enabled"), value.enabled);
	return object;
}

bool Validate(
		const LinkRule &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Matches(value.id, QString::fromUtf8("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("id")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.host.toUcs4().size() <= 253 && Codec::Matches(value.host, QString::fromUtf8("^[a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?(\\.[a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?)*$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("host")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.replacementHost.toUcs4().size() <= 253 && Codec::Matches(value.replacementHost, QString::fromUtf8("^([a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?(\\.[a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?)*)?$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("replacementHost")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(qsizetype(value.removeParameters.size()) <= 32)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("removeParameters")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Unique(value.removeParameters))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("removeParameters")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.removeParameters.size()); ++i) {
		const auto &item = value.removeParameters[i];
		if (!(Codec::Matches(item, QString::fromUtf8("^[a-zA-Z0-9_+.-]{1,64}\\*?$")))) {
			return Codec::Fail(error, Codec::Item(Codec::Child(path, QLatin1StringView("removeParameters")), i), QString::fromLatin1("violates the schema rules"));
		}
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		LinkRules &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("confirmAll"),
			QLatin1StringView("rules"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("confirmAll"),
			QLatin1StringView("rules"),
		}, error, path)) {
		return false;
	}
	result = LinkRules();
	return true
		&& Codec::ReadField(object, QLatin1StringView("confirmAll"), result.confirmAll, error, path)
		&& Codec::ReadField(object, QLatin1StringView("rules"), result.rules, error, path);
}

QJsonValue Write(const LinkRules &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("confirmAll"), value.confirmAll);
	Codec::WriteField(object, QLatin1StringView("rules"), value.rules);
	return object;
}

bool Validate(
		const LinkRules &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.rules.size()) <= 32)) {
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

std::optional<LinkRules> ParseLinkRules(
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
	auto result = LinkRules();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidLinkRules(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeLinkRules(const LinkRules &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Links
