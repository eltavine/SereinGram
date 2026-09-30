// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <rpl/producer.h>

namespace Serein::Hooks::ServiceSettings {

[[nodiscard]] QByteArray ServicesConfig();
[[nodiscard]] rpl::producer<QByteArray> ServicesConfigValue();
[[nodiscard]] bool PreferSystemAi();
[[nodiscard]] rpl::producer<bool> PreferSystemAiValue();

} // namespace Serein::Hooks::ServiceSettings
