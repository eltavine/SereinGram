#include "serein/services/model.h"

#include <QtCore/QJsonDocument>

namespace Serein {

std::optional<QJsonObject> Services() {
	const auto bytes = ForDevice().Get(kServicesConfig);
	if (ForDevice().invalidKeys().contains(kServicesConfig.key)) {
		LOG(("Serein Error: Invalid services configuration; external services disabled."));
		return std::nullopt;
	}
	if (bytes.isEmpty()) {
		return ServicesDefaults();
	}
	const auto document = QJsonDocument::fromJson(bytes);
	if (!document.isObject() || !ValidServices(document.object())) {
		LOG(("Serein Error: Invalid services configuration; external services disabled."));
		return std::nullopt;
	}
	return document.object();
}

bool SetServices(const QJsonObject &value) {
	return ValidServices(value) && ForDevice().Set(kServicesConfig,
		value == ServicesDefaults() ? QByteArray()
			: QJsonDocument(value).toJson(QJsonDocument::Compact));
}

} // namespace Serein
