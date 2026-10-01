// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <rpl/producer.h>

namespace Serein::Hooks::ServiceSettings {

[[nodiscard]] QByteArray ServicesConfig();
[[nodiscard]] rpl::producer<QByteArray> ServicesConfigValue();
[[nodiscard]] bool PreferSystemAi();
[[nodiscard]] rpl::producer<bool> PreferSystemAiValue();
[[nodiscard]] QString ProxySubscription();
[[nodiscard]] rpl::producer<QString> ProxySubscriptionValue();
[[nodiscard]] bool PauseProxyOnVpn();
[[nodiscard]] rpl::producer<bool> PauseProxyOnVpnValue();
[[nodiscard]] bool ProxyPausedByVpn();
[[nodiscard]] rpl::producer<bool> ProxyPausedByVpnValue();

} // namespace Serein::Hooks::ServiceSettings
