// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"
#include "serein/schema/codec.h"

namespace Serein::ServiceSettings {

[[nodiscard]] bool ValidServicesBytes(const QByteArray &value);

inline const auto kServicesConfig = Option<QByteArray>{
	"serein.services",
	Scope::Device,
	QByteArray(),
	Category::Services,
	"lng_serein_services",
	0,
	&ValidServicesBytes };
inline constexpr auto kPreferSystemAi = Option<bool>{
	"serein.preferSystemAi",
	Scope::Device,
	false,
	Category::Services,
	"lng_serein_system_ai",
	static_cast<unsigned>(Flag::RefreshComposeButtons) };
inline const auto kProxySubscription = Option<QString>{
	"serein.proxySubscription",
	Scope::Device,
	QString(),
	Category::Services,
	"lng_serein_proxy_subscription",
	0,
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 2048) && (Codec::Matches(value, QString::fromUtf8("^(https://[^\\s]+)?$"))));
	} };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kServicesConfig));
	Expects(registry.Add(kPreferSystemAi));
	Expects(registry.Add(kProxySubscription));
}

} // namespace Serein::ServiceSettings
