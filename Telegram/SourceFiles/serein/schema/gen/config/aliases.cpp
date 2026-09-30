// Generated from proto/serein/config/v1/aliases.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/aliases.h"

namespace Serein::Privacy {

bool Read(
		const QJsonValue &json,
		PeerAliasesConfig &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("names"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("names"),
		}, error, path)) {
		return false;
	}
	result = PeerAliasesConfig();
	return true
		&& Codec::ReadField(object, QLatin1StringView("names"), result.names, error, path);
}

QJsonValue Write(const PeerAliasesConfig &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("names"), value.names);
	return object;
}

bool Validate(
		const PeerAliasesConfig &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.names.size()) <= 1000)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("names")), QString::fromLatin1("violates the schema rules"));
	}
	for (const auto &[key, item] : value.names) {
		if (!(Codec::Matches(key, QString::fromUtf8("^[1-9][0-9]{0,19}$")))) {
			return Codec::Fail(error, Codec::Entry(Codec::Child(path, QLatin1StringView("names")), key), QString::fromLatin1("violates the schema rules"));
		}
		if (!(item.toUcs4().size() >= 1 && item.toUcs4().size() <= 96)) {
			return Codec::Fail(error, Codec::Entry(Codec::Child(path, QLatin1StringView("names")), key), QString::fromLatin1("violates the schema rules"));
		}
	}
	return true;
}

std::optional<PeerAliasesConfig> ParsePeerAliasesConfig(
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
	auto result = PeerAliasesConfig();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidPeerAliasesConfig(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializePeerAliasesConfig(const PeerAliasesConfig &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Privacy
