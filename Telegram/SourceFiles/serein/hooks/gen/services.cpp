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

bool TranslationContext() {
	return ForDevice().Get(Serein::ServiceSettings::kTranslationContext);
}

rpl::producer<bool> TranslationContextValue() {
	return ForDevice().Value(Serein::ServiceSettings::kTranslationContext);
}

bool ChatTranslationWithoutPremium() {
	return ForDevice().Get(Serein::ServiceSettings::kChatTranslationWithoutPremium);
}

rpl::producer<bool> ChatTranslationWithoutPremiumValue() {
	return ForDevice().Value(Serein::ServiceSettings::kChatTranslationWithoutPremium);
}

bool AutoTranslateChats(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::ServiceSettings::kAutoTranslateChats);
}

rpl::producer<bool> AutoTranslateChatsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::ServiceSettings::kAutoTranslateChats);
}

bool InstantViewTranslation() {
	return ForDevice().Get(Serein::ServiceSettings::kInstantViewTranslation);
}

rpl::producer<bool> InstantViewTranslationValue() {
	return ForDevice().Value(Serein::ServiceSettings::kInstantViewTranslation);
}

QString ProxySubscription() {
	return ForDevice().Get(Serein::ServiceSettings::kProxySubscription);
}

rpl::producer<QString> ProxySubscriptionValue() {
	return ForDevice().Value(Serein::ServiceSettings::kProxySubscription);
}

QByteArray ProxyNotes() {
	return ForDevice().Get(Serein::ServiceSettings::kProxyNotes);
}

rpl::producer<QByteArray> ProxyNotesValue() {
	return ForDevice().Value(Serein::ServiceSettings::kProxyNotes);
}

bool PauseProxyOnVpn() {
	return ForDevice().Get(Serein::ServiceSettings::kPauseProxyOnVpn);
}

rpl::producer<bool> PauseProxyOnVpnValue() {
	return ForDevice().Value(Serein::ServiceSettings::kPauseProxyOnVpn);
}

bool ProxyPausedByVpn() {
	return ForDevice().Get(Serein::ServiceSettings::kProxyPausedByVpn);
}

rpl::producer<bool> ProxyPausedByVpnValue() {
	return ForDevice().Value(Serein::ServiceSettings::kProxyPausedByVpn);
}

QString CustomDoh() {
	return ForDevice().Get(Serein::ServiceSettings::kCustomDoh);
}

rpl::producer<QString> CustomDohValue() {
	return ForDevice().Value(Serein::ServiceSettings::kCustomDoh);
}

bool AndroidWebApps() {
	return ForDevice().Get(Serein::ServiceSettings::kAndroidWebApps);
}

rpl::producer<bool> AndroidWebAppsValue() {
	return ForDevice().Value(Serein::ServiceSettings::kAndroidWebApps);
}

bool FasterTransfers() {
	return ForDevice().Get(Serein::ServiceSettings::kFasterTransfers);
}

rpl::producer<bool> FasterTransfersValue() {
	return ForDevice().Value(Serein::ServiceSettings::kFasterTransfers);
}

} // namespace Serein::Hooks::ServiceSettings
