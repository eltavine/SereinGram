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
inline constexpr auto kTranslationContext = Option<bool>{
	"serein.translationContext",
	Scope::Device,
	false,
	Category::Services,
	"lng_serein_translation_context",
	0 };
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
inline constexpr auto kPauseProxyOnVpn = Option<bool>{
	"serein.pauseProxyOnVpn",
	Scope::Device,
	false,
	Category::Services,
	"lng_serein_proxy_vpn",
	0 };
inline constexpr auto kProxyPausedByVpn = Option<bool>{
	"serein.proxyPausedByVpn",
	Scope::Device,
	false,
	Category::Services,
	"lng_serein_proxy_vpn",
	static_cast<unsigned>(Flag::Hidden) };
inline const auto kCustomDoh = Option<QString>{
	"serein.customDoh",
	Scope::Device,
	QString(),
	Category::Services,
	"lng_serein_custom_doh",
	0,
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 253) && (Codec::Matches(value, QString::fromUtf8("^(([A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?\\.)+[A-Za-z]{2,63})?$"))));
	} };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kServicesConfig));
	Expects(registry.Add(kPreferSystemAi));
	Expects(registry.Add(kTranslationContext));
	Expects(registry.Add(kProxySubscription));
	Expects(registry.Add(kPauseProxyOnVpn));
	Expects(registry.Add(kProxyPausedByVpn));
	Expects(registry.Add(kCustomDoh));
}

} // namespace Serein::ServiceSettings
