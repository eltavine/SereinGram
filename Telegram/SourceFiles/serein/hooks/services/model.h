#pragma once

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"

#include <QtCore/QJsonObject>
#include <QtCore/QUrl>

#include <optional>
#include <utility>
#include <vector>

namespace Serein {

using ServiceSettings::kPreferSystemAi;
using ServiceSettings::kServicesConfig;

enum class ServiceKind {
	Translation,
	Transcription,
};

struct ServiceDefinition {
	QString id;
	QString name;
	ServiceKind kind = ServiceKind::Translation;
	QString protocol;
	QUrl baseUrl;
	QString endpoint;
	QString model;
	QString credentialRef;
	bool useKey = true;
	QString systemPrompt;
	QString prompt;
	QString language;
	std::optional<double> temperature;
	QString region;
};

[[nodiscard]] QJsonObject ServicesDefaults();
[[nodiscard]] QJsonObject UpgradeServices(QJsonObject value);
[[nodiscard]] std::optional<ServiceDefinition> ParseService(const QJsonObject &value);
[[nodiscard]] QJsonObject SerializeService(const ServiceDefinition &value);
[[nodiscard]] bool ValidServices(const QJsonObject &value);
[[nodiscard]] std::optional<QJsonObject> Services();
[[nodiscard]] bool SetServices(const QJsonObject &value);
[[nodiscard]] std::optional<ServiceDefinition> FindService(
	const QJsonObject &settings,
	const QString &id);
[[nodiscard]] bool IsLanguageModelProtocol(const QString &protocol);
[[nodiscard]] bool IsKeylessProtocol(const QString &protocol);
[[nodiscard]] std::vector<std::pair<QByteArray, QByteArray>> ServiceHeaders(
	const ServiceDefinition &service);
[[nodiscard]] std::pair<QByteArray, QByteArray> ServiceAuthorization(
	const ServiceDefinition &service,
	const QByteArray &secret);
[[nodiscard]] QString CredentialAccount(const ServiceDefinition &service);
[[nodiscard]] QUrl ServiceEndpoint(const ServiceDefinition &service);

} // namespace Serein
