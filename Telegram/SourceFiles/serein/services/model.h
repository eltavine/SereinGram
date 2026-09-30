#pragma once

#include "serein/core/options.h"

#include <QtCore/QJsonObject>
#include <QtCore/QUrl>

#include <optional>

namespace Serein {

[[nodiscard]] bool ValidServicesBytes(const QByteArray &raw);
inline const auto kServicesConfig = Option<QByteArray>{
	"serein.services", Scope::Device, QByteArray(),
	Category::Services, "lng_serein_services", 0, ValidServicesBytes };
inline constexpr auto kPreferSystemAi = Option<bool>{
	"serein.preferSystemAi", Scope::Device, false,
	Category::Services, "lng_serein_system_ai",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };

inline void RegisterServiceOptions(Registry &registry) {
	Expects(registry.Add(kServicesConfig));
	Expects(registry.Add(kPreferSystemAi));
}

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
};

[[nodiscard]] QJsonObject ServicesDefaults();
[[nodiscard]] std::optional<ServiceDefinition> ParseService(const QJsonObject &value);
[[nodiscard]] QJsonObject SerializeService(const ServiceDefinition &value);
[[nodiscard]] bool ValidServices(const QJsonObject &value);
[[nodiscard]] std::optional<QJsonObject> Services();
[[nodiscard]] bool SetServices(const QJsonObject &value);
[[nodiscard]] std::optional<ServiceDefinition> FindService(
	const QJsonObject &settings,
	const QString &id);
[[nodiscard]] QString CredentialAccount(const ServiceDefinition &service);
[[nodiscard]] QUrl ServiceEndpoint(const ServiceDefinition &service);

} // namespace Serein
