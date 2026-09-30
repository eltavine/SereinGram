// Generated from proto/serein/config/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::ServicesSchema {

struct ServiceInstance {
	QString id;
	QString name;
	QString kind;
	QString protocol;
	QString baseUrl;
	QString endpoint;
	QString model;
	QString credentialRef;
	bool useKey = false;
	QString systemPrompt;
	QString prompt;
	QString language;
	std::optional<double> temperature;
	QString region;

	friend bool operator==(const ServiceInstance &, const ServiceInstance &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	ServiceInstance &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const ServiceInstance &value);
[[nodiscard]] bool Validate(
	const ServiceInstance &value,
	Codec::Error &error,
	const QString &path);

struct ServicesConfig {
	QString translation;
	QString transcription;
	std::vector<ServiceInstance> instances;

	friend bool operator==(const ServicesConfig &, const ServicesConfig &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	ServicesConfig &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const ServicesConfig &value);
[[nodiscard]] bool Validate(
	const ServicesConfig &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidServicesConfig(const ServicesConfig &value);
[[nodiscard]] std::optional<ServicesConfig> ParseServicesConfig(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeServicesConfig(const ServicesConfig &value);

} // namespace Serein::ServicesSchema
