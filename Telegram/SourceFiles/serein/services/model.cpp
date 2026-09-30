#include "serein/hooks/services/model.h"
#include "base/basic_types.h"
#include "base/flat_map.h"
#include "serein/schema/gen/config/services.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>

#include <cmath>

namespace Serein {
namespace {

bool ValidText(const QString &text, int maximum, bool multiline = false) {
	return text.size() <= maximum
		&& QString::fromUtf8(text.toUtf8()) == text
		&& !text.contains(QChar(0))
		&& (multiline || (!text.contains('\n') && !text.contains('\r')));
}

bool ValidEndpoint(const QUrl &url, const QString &endpoint) {
	const auto relative = QUrl(endpoint, QUrl::StrictMode);
	const auto decoded = QUrl::fromPercentEncoding(endpoint.toUtf8());
	const auto loopback = (url.host() == u"localhost"_q
		|| url.host() == u"127.0.0.1"_q
		|| url.host() == u"::1"_q);
	return url.isValid() && !url.host().isEmpty()
		&& (url.scheme() == u"https"_q || (loopback && url.scheme() == u"http"_q))
		&& url.userInfo().isEmpty() && !url.hasQuery() && !url.hasFragment()
		&& relative.isValid() && relative.isRelative()
		&& !endpoint.isEmpty() && !endpoint.startsWith('/')
		&& !endpoint.contains('\\') && relative.authority().isEmpty()
		&& !decoded.contains('\\')
		&& !relative.hasQuery() && !relative.hasFragment()
		&& !endpoint.split('/').contains(u".."_q)
		&& !decoded.split('/').contains(u".."_q);
}

std::optional<ServiceDefinition> Definition(
		const ServicesSchema::ServiceInstance &value) {
	const auto translation = (value.kind == u"translation"_q);
	const auto model = IsLanguageModelProtocol(value.protocol);
	const auto hotter = value.temperature && (*value.temperature > 1);
	if (QUuid(value.id).isNull() || QUuid(value.credentialRef).isNull()
		|| !ValidText(value.name, 256)
		|| !ValidText(value.model, 256)
		|| !ValidText(value.language, 256)
		|| !ValidText(value.baseUrl, 2048)
		|| !ValidText(value.endpoint, 2048)
		|| !ValidText(value.systemPrompt, 16384, true)
		|| !ValidText(value.prompt, 16384, true)
		|| value.name.trimmed().isEmpty()
		|| (!translation && value.protocol != u"openai"_q)
		|| (IsKeylessProtocol(value.protocol) && value.useKey)
		|| (model && value.model.trimmed().isEmpty())
		|| (!model && (!value.model.isEmpty()
			|| !value.systemPrompt.isEmpty()
			|| !value.prompt.isEmpty()
			|| value.temperature))
		|| (!translation && !value.systemPrompt.isEmpty())
		|| (translation && !value.language.isEmpty())
		|| (hotter && (!translation || value.protocol == u"anthropic"_q))) {
		return std::nullopt;
	}
	const auto url = QUrl(value.baseUrl, QUrl::StrictMode);
	if (!ValidEndpoint(url, value.endpoint)) {
		return std::nullopt;
	}
	return ServiceDefinition{
		.id = value.id,
		.name = value.name,
		.kind = translation ? ServiceKind::Translation : ServiceKind::Transcription,
		.protocol = value.protocol,
		.baseUrl = url,
		.endpoint = value.endpoint,
		.model = value.model,
		.credentialRef = value.credentialRef,
		.useKey = value.useKey,
		.systemPrompt = value.systemPrompt,
		.prompt = value.prompt,
		.language = value.language,
		.temperature = value.temperature,
	};
}

} // namespace

bool IsLanguageModelProtocol(const QString &protocol) {
	return (protocol == u"openai"_q) || (protocol == u"anthropic"_q);
}

bool IsKeylessProtocol(const QString &protocol) {
	return (protocol == u"google"_q)
		|| (protocol == u"yandex"_q)
		|| (protocol == u"transmart"_q);
}

std::vector<std::pair<QByteArray, QByteArray>> ServiceHeaders(
		const ServiceDefinition &service) {
	if (service.protocol == u"anthropic"_q) {
		return { { "anthropic-version", "2023-06-01" } };
	}
	return {};
}

std::pair<QByteArray, QByteArray> ServiceAuthorization(
		const ServiceDefinition &service,
		const QByteArray &secret) {
	if (service.protocol == u"anthropic"_q) {
		return { "x-api-key", secret };
	} else if (service.protocol == u"deepl"_q) {
		return { "Authorization", "DeepL-Auth-Key " + secret };
	}
	return { "Authorization", "Bearer " + secret };
}

QJsonObject ServicesDefaults() {
	return {
		{ u"version"_q, 1 },
		{ u"translation"_q, QString() },
		{ u"transcription"_q, QString() },
		{ u"instances"_q, QJsonArray() },
	};
}

QJsonObject SerializeService(const ServiceDefinition &value) {
	auto instance = ServicesSchema::ServiceInstance();
	instance.id = value.id;
	instance.name = value.name;
	instance.kind = (value.kind == ServiceKind::Translation)
		? u"translation"_q
		: u"transcription"_q;
	instance.protocol = value.protocol;
	instance.baseUrl = value.baseUrl.toString(QUrl::FullyEncoded);
	instance.endpoint = value.endpoint;
	instance.model = value.model;
	instance.credentialRef = value.credentialRef;
	instance.useKey = value.useKey;
	instance.systemPrompt = value.systemPrompt;
	instance.prompt = value.prompt;
	instance.language = value.language;
	instance.temperature = value.temperature;
	return ServicesSchema::Write(instance).toObject();
}

std::optional<ServiceDefinition> ParseService(const QJsonObject &value) {
	auto error = Codec::Error();
	auto instance = ServicesSchema::ServiceInstance();
	if (!ServicesSchema::Read(QJsonValue(value), instance, error, QString())
		|| !ServicesSchema::Validate(instance, error, QString())) {
		return std::nullopt;
	}
	return Definition(instance);
}

bool ValidServices(const QJsonObject &value) {
	return ServicesSchema::ParseServicesConfig(
		QJsonDocument(value).toJson(QJsonDocument::Compact)).has_value();
}

bool ValidServicesBytes(const QByteArray &raw) {
	return raw.isEmpty() || ServicesSchema::ParseServicesConfig(raw).has_value();
}

std::optional<ServiceDefinition> FindService(const QJsonObject &settings, const QString &id) {
	for (const auto &entry : settings.value(u"instances"_q).toArray()) {
		if (entry.toObject().value(u"id"_q) == id) {
			return ParseService(entry.toObject());
		}
	}
	return std::nullopt;
}

QUrl ServiceEndpoint(const ServiceDefinition &service) {
	auto base = service.baseUrl;
	if (!base.path().endsWith('/')) {
		base.setPath(base.path() + '/');
	}
	return base.resolved(QUrl(service.endpoint, QUrl::StrictMode));
}

QString CredentialAccount(const ServiceDefinition &service) {
	const auto binding = QJsonDocument(QJsonArray{
		ServiceEndpoint(service).toString(QUrl::FullyEncoded),
		service.protocol,
		service.kind == ServiceKind::Translation ? u"translation"_q : u"transcription"_q,
	}).toJson(QJsonDocument::Compact);
	return service.credentialRef + '.' + QString::fromLatin1(
		QCryptographicHash::hash(binding, QCryptographicHash::Sha256).toHex());
}

} // namespace Serein

namespace Serein::ServicesSchema {

bool ValidServicesConfig(const ServicesConfig &value) {
	auto kinds = base::flat_map<QString, ServiceKind>();
	for (const auto &instance : value.instances) {
		const auto definition = Definition(instance);
		if (!definition || !kinds.emplace(definition->id, definition->kind).second) {
			return false;
		}
	}
	const auto selected = [&](const QString &id, ServiceKind kind, bool system) {
		if (id.isEmpty() || id == u"telegram"_q || (system && id == u"system"_q)) {
			return true;
		}
		const auto i = kinds.find(id);
		return (i != kinds.end()) && (i->second == kind);
	};
	return selected(value.translation, ServiceKind::Translation, true)
		&& selected(value.transcription, ServiceKind::Transcription, false);
}

} // namespace Serein::ServicesSchema
