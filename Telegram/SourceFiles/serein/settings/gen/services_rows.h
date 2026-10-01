// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::ServiceSettings {


struct CustomRows {
	CustomRow services;
	CustomRow preferSystemAi;
	CustomRow translationContext;
	CustomRow chatTranslationWithoutPremium;
	CustomRow autoTranslateChats;
	CustomRow proxySubscription;
	CustomRow proxyNotes;
	CustomRow pauseProxyOnVpn;
	CustomRow customDoh;
	CustomRow androidWebApps;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.services();
	custom.preferSystemAi();
	custom.translationContext();
	custom.chatTranslationWithoutPremium();
	custom.autoTranslateChats();
	custom.proxySubscription();
	custom.proxyNotes();
	custom.pauseProxyOnVpn();
	custom.customDoh();
	custom.androidWebApps();
}

} // namespace Serein::ServiceSettings
