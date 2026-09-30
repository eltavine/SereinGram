#include "serein/hooks/services/model.h"

namespace Serein {

std::optional<ServicesConfig> Services() {
	if (ForDevice().invalidKeys().contains(kServicesConfig.key)) {
		LOG(("Serein Error: Invalid services configuration; external services disabled."));
		return std::nullopt;
	}
	auto result = ReadServices(ForDevice().Get(kServicesConfig));
	if (!result) {
		LOG(("Serein Error: Invalid services configuration; external services disabled."));
	}
	return result;
}

bool SetServices(const ServicesConfig &value) {
	const auto raw = WriteServices(value);
	return ServiceSettings::ValidServicesBytes(raw)
		&& ForDevice().Set(kServicesConfig, raw);
}

} // namespace Serein
