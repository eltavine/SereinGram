// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::ServiceSettings {

[[nodiscard]] QByteArray ServicesConfig();
[[nodiscard]] rpl::producer<QByteArray> ServicesConfigValue();
[[nodiscard]] bool PreferSystemAi();
[[nodiscard]] rpl::producer<bool> PreferSystemAiValue();
[[nodiscard]] bool TranslationContext();
[[nodiscard]] rpl::producer<bool> TranslationContextValue();
[[nodiscard]] bool ChatTranslationWithoutPremium();
[[nodiscard]] rpl::producer<bool> ChatTranslationWithoutPremiumValue();
[[nodiscard]] bool AutoTranslateChats(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> AutoTranslateChatsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] QString ProxySubscription();
[[nodiscard]] rpl::producer<QString> ProxySubscriptionValue();
[[nodiscard]] QByteArray ProxyNotes();
[[nodiscard]] rpl::producer<QByteArray> ProxyNotesValue();
[[nodiscard]] bool PauseProxyOnVpn();
[[nodiscard]] rpl::producer<bool> PauseProxyOnVpnValue();
[[nodiscard]] bool ProxyPausedByVpn();
[[nodiscard]] rpl::producer<bool> ProxyPausedByVpnValue();
[[nodiscard]] QString CustomDoh();
[[nodiscard]] rpl::producer<QString> CustomDohValue();
[[nodiscard]] bool AndroidWebApps();
[[nodiscard]] rpl::producer<bool> AndroidWebAppsValue();

} // namespace Serein::Hooks::ServiceSettings
