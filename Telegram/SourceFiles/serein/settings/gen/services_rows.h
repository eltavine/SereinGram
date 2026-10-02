// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::ServiceSettings {

inline const auto kToggleRows = std::array<ToggleRow, 7>{ {
	{
		&kTranslationContext,
		tr::lng_serein_translation_context,
		u"serein/services/translation-context"_q,
		{ u"LLM"_q, u"context"_q, u"translate"_q },
	},
	{
		&kChatTranslationWithoutPremium,
		tr::lng_serein_chat_translation,
		u"serein/services/chat-translation-without-premium"_q,
		{ u"translate"_q, u"chat"_q, u"Premium"_q },
	},
	{
		&kAutoTranslateChats,
		tr::lng_serein_auto_translate_chats,
		u"serein/services/auto-translate-chats"_q,
		{ u"translate"_q, u"automatic"_q, u"chat"_q },
	},
	{
		&kInstantViewTranslation,
		tr::lng_serein_instant_view_translation,
		u"serein/services/instant-view-translation"_q,
		{ u"translate"_q, u"Instant View"_q, u"article"_q },
	},
	{
		&kPauseProxyOnVpn,
		tr::lng_serein_proxy_vpn,
		u"serein/services/pause-proxy-on-vpn"_q,
		{ u"proxy"_q, u"VPN"_q },
	},
	{
		&kFasterTransfers,
		tr::lng_serein_faster_transfers,
		u"serein/services/faster-transfers"_q,
		{ u"upload"_q, u"download"_q, u"speed"_q },
	},
	{
		&kAndroidWebApps,
		tr::lng_serein_android_web_apps,
		u"serein/services/android-web-apps"_q,
		{ u"mini apps"_q, u"web apps"_q, u"bots"_q, u"Android"_q },
	},
} };

struct CustomRows {
	CustomRow services;
	CustomRow preferSystemAi;
	CustomRow proxySubscription;
	CustomRow proxyNotes;
	CustomRow customDoh;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.services();
	custom.preferSystemAi();
	AddToggle(builder, kToggleRows[0]);
	AddNote(builder, tr::lng_serein_translation_context_about);
	AddToggle(builder, kToggleRows[1]);
	AddNote(builder, tr::lng_serein_chat_translation_about);
	AddToggle(builder, kToggleRows[2]);
	AddNote(builder, tr::lng_serein_auto_translate_chats_about);
	AddToggle(builder, kToggleRows[3]);
	AddNote(builder, tr::lng_serein_instant_view_translation_about);
	custom.proxySubscription();
	custom.proxyNotes();
	AddToggle(builder, kToggleRows[4]);
	AddNote(builder, tr::lng_serein_proxy_vpn_about);
	custom.customDoh();
	AddToggle(builder, kToggleRows[5]);
	AddNote(builder, tr::lng_serein_faster_transfers_about);
	AddToggle(builder, kToggleRows[6]);
	AddNote(builder, tr::lng_serein_android_web_apps_about);
}

} // namespace Serein::ServiceSettings
