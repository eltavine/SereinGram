// Generated from proto/serein/config/v1/services.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/services.h"

namespace Serein::ServicesSchema {

bool Read(
		const QJsonValue &json,
		ServiceInstance &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("id"),
			QLatin1StringView("name"),
			QLatin1StringView("kind"),
			QLatin1StringView("protocol"),
			QLatin1StringView("baseUrl"),
			QLatin1StringView("endpoint"),
			QLatin1StringView("model"),
			QLatin1StringView("credentialRef"),
			QLatin1StringView("useKey"),
			QLatin1StringView("systemPrompt"),
			QLatin1StringView("prompt"),
			QLatin1StringView("language"),
			QLatin1StringView("temperature"),
			QLatin1StringView("region"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("id"),
			QLatin1StringView("name"),
			QLatin1StringView("kind"),
			QLatin1StringView("protocol"),
			QLatin1StringView("baseUrl"),
			QLatin1StringView("endpoint"),
			QLatin1StringView("model"),
			QLatin1StringView("credentialRef"),
			QLatin1StringView("useKey"),
			QLatin1StringView("systemPrompt"),
			QLatin1StringView("prompt"),
			QLatin1StringView("language"),
			QLatin1StringView("temperature"),
			QLatin1StringView("region"),
		}, error, path)) {
		return false;
	}
	result = ServiceInstance();
	return true
		&& Codec::ReadField(object, QLatin1StringView("id"), result.id, error, path)
		&& Codec::ReadField(object, QLatin1StringView("name"), result.name, error, path)
		&& Codec::ReadField(object, QLatin1StringView("kind"), result.kind, error, path)
		&& Codec::ReadField(object, QLatin1StringView("protocol"), result.protocol, error, path)
		&& Codec::ReadField(object, QLatin1StringView("baseUrl"), result.baseUrl, error, path)
		&& Codec::ReadField(object, QLatin1StringView("endpoint"), result.endpoint, error, path)
		&& Codec::ReadField(object, QLatin1StringView("model"), result.model, error, path)
		&& Codec::ReadField(object, QLatin1StringView("credentialRef"), result.credentialRef, error, path)
		&& Codec::ReadField(object, QLatin1StringView("useKey"), result.useKey, error, path)
		&& Codec::ReadField(object, QLatin1StringView("systemPrompt"), result.systemPrompt, error, path)
		&& Codec::ReadField(object, QLatin1StringView("prompt"), result.prompt, error, path)
		&& Codec::ReadField(object, QLatin1StringView("language"), result.language, error, path)
		&& Codec::ReadField(object, QLatin1StringView("temperature"), result.temperature, error, path)
		&& Codec::ReadField(object, QLatin1StringView("region"), result.region, error, path);
}

QJsonValue Write(const ServiceInstance &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("id"), value.id);
	Codec::WriteField(object, QLatin1StringView("name"), value.name);
	Codec::WriteField(object, QLatin1StringView("kind"), value.kind);
	Codec::WriteField(object, QLatin1StringView("protocol"), value.protocol);
	Codec::WriteField(object, QLatin1StringView("baseUrl"), value.baseUrl);
	Codec::WriteField(object, QLatin1StringView("endpoint"), value.endpoint);
	Codec::WriteField(object, QLatin1StringView("model"), value.model);
	Codec::WriteField(object, QLatin1StringView("credentialRef"), value.credentialRef);
	Codec::WriteField(object, QLatin1StringView("useKey"), value.useKey);
	Codec::WriteField(object, QLatin1StringView("systemPrompt"), value.systemPrompt);
	Codec::WriteField(object, QLatin1StringView("prompt"), value.prompt);
	Codec::WriteField(object, QLatin1StringView("language"), value.language);
	Codec::WriteNullableField(object, QLatin1StringView("temperature"), value.temperature);
	Codec::WriteField(object, QLatin1StringView("region"), value.region);
	return object;
}

bool Validate(
		const ServiceInstance &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Matches(value.id, QString::fromUtf8("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("id")), QString::fromLatin1("violates the schema rules"));
	}
	if (!((value.kind == QString::fromUtf8("translation") || value.kind == QString::fromUtf8("transcription")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("kind")), QString::fromLatin1("violates the schema rules"));
	}
	if (!((value.protocol == QString::fromUtf8("openai") || value.protocol == QString::fromUtf8("anthropic") || value.protocol == QString::fromUtf8("deepl") || value.protocol == QString::fromUtf8("deeplx") || value.protocol == QString::fromUtf8("google") || value.protocol == QString::fromUtf8("yandex") || value.protocol == QString::fromUtf8("transmart") || value.protocol == QString::fromUtf8("azure")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("protocol")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Matches(value.credentialRef, QString::fromUtf8("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("credentialRef")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Matches(value.language, QString::fromUtf8("^([a-z]{2})?$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("language")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(!value.temperature || ((*value.temperature) <= 2.0 && (*value.temperature) >= 0.0))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("temperature")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Matches(value.region, QString::fromUtf8("^([a-z0-9]{1,32})?$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("region")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		ServicesConfig &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("translation"),
			QLatin1StringView("transcription"),
			QLatin1StringView("instances"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("translation"),
			QLatin1StringView("transcription"),
			QLatin1StringView("instances"),
		}, error, path)) {
		return false;
	}
	result = ServicesConfig();
	return true
		&& Codec::ReadField(object, QLatin1StringView("translation"), result.translation, error, path)
		&& Codec::ReadField(object, QLatin1StringView("transcription"), result.transcription, error, path)
		&& Codec::ReadField(object, QLatin1StringView("instances"), result.instances, error, path);
}

QJsonValue Write(const ServicesConfig &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("translation"), value.translation);
	Codec::WriteField(object, QLatin1StringView("transcription"), value.transcription);
	Codec::WriteField(object, QLatin1StringView("instances"), value.instances);
	return object;
}

bool Validate(
		const ServicesConfig &value,
		Codec::Error &error,
		const QString &path) {
	for (auto i = qsizetype(); i != qsizetype(value.instances.size()); ++i) {
		const auto &item = value.instances[i];
		if (!Validate(item, error, Codec::Item(Codec::Child(path, QLatin1StringView("instances")), i))) {
			return false;
		}
	}
	return true;
}

std::optional<ServicesConfig> ParseServicesConfig(
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
	auto result = ServicesConfig();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidServicesConfig(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeServicesConfig(const ServicesConfig &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 2);
	return Codec::Serialize(object);
}

bool Read(
		const QJsonValue &json,
		SendTranslations &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("languages"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("languages"),
		}, error, path)) {
		return false;
	}
	result = SendTranslations();
	return true
		&& Codec::ReadField(object, QLatin1StringView("languages"), result.languages, error, path);
}

QJsonValue Write(const SendTranslations &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("languages"), value.languages);
	return object;
}

bool Validate(
		const SendTranslations &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.languages.size()) <= 500)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("languages")), QString::fromLatin1("violates the schema rules"));
	}
	for (const auto &[key, item] : value.languages) {
		if (!(Codec::Matches(key, QString::fromUtf8("^[1-9][0-9]{0,19}$")))) {
			return Codec::Fail(error, Codec::Entry(Codec::Child(path, QLatin1StringView("languages")), key), QString::fromLatin1("violates the schema rules"));
		}
		if (!(Codec::Matches(item, QString::fromUtf8("^[a-z]{2,3}(_[A-Za-z0-9]{2,8}){0,2}$")))) {
			return Codec::Fail(error, Codec::Entry(Codec::Child(path, QLatin1StringView("languages")), key), QString::fromLatin1("violates the schema rules"));
		}
	}
	return true;
}

std::optional<SendTranslations> ParseSendTranslations(
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
	auto result = SendTranslations();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeSendTranslations(const SendTranslations &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::ServicesSchema
