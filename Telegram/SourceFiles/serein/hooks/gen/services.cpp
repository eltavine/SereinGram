// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/services.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"

namespace Serein::Hooks::ServiceSettings {

QByteArray ServicesConfig() {
	return ForDevice().Get(Serein::ServiceSettings::kServicesConfig);
}

rpl::producer<QByteArray> ServicesConfigValue() {
	return ForDevice().Value(Serein::ServiceSettings::kServicesConfig);
}

bool PreferSystemAi() {
	return ForDevice().Get(Serein::ServiceSettings::kPreferSystemAi);
}

rpl::producer<bool> PreferSystemAiValue() {
	return ForDevice().Value(Serein::ServiceSettings::kPreferSystemAi);
}

} // namespace Serein::Hooks::ServiceSettings
