// Generated from proto/serein/settings/v1/services.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::ServiceSettings {

inline const auto kToggleRows = std::array<ToggleRow, 8>{ {
	{
		.option = &kTranslationContext,
		.title = tr::lng_serein_translation_context,
		.id = u"serein/services/translation-context"_q,
		.keywords = { u"LLM"_q, u"context"_q, u"translate"_q },
		.icon = &st::menuIconChats,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_translation_context_summary,
	},
	{
		.option = &kChatTranslationWithoutPremium,
		.title = tr::lng_serein_chat_translation,
		.id = u"serein/services/chat-translation-without-premium"_q,
		.keywords = { u"translate"_q, u"chat"_q, u"Premium"_q },
		.icon = &st::menuIconPremium,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_chat_translation_without_premium_about,
	},
	{
		.option = &kAutoTranslateChats,
		.title = tr::lng_serein_auto_translate_chats,
		.id = u"serein/services/auto-translate-chats"_q,
		.keywords = { u"translate"_q, u"automatic"_q, u"chat"_q },
		.icon = &st::menuIconAsTopics,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_auto_translate_chats_summary,
	},
	{
		.option = &kInstantViewTranslation,
		.title = tr::lng_serein_instant_view_translation,
		.id = u"serein/services/instant-view-translation"_q,
		.keywords = { u"translate"_q, u"Instant View"_q, u"article"_q },
		.icon = &st::menuIconArticle,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_instant_view_translation_summary,
	},
	{
		.option = &kPauseProxyOnVpn,
		.title = tr::lng_serein_proxy_vpn,
		.id = u"serein/services/pause-proxy-on-vpn"_q,
		.keywords = { u"proxy"_q, u"VPN"_q },
		.icon = &st::menuIcon2SV,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_pause_proxy_on_vpn_about,
	},
	{
		.option = &kSystemDns,
		.title = tr::lng_serein_system_dns,
		.id = u"serein/services/system-dns"_q,
		.keywords = { u"DNS"_q, u"DoH"_q, u"proxy"_q, u"Google"_q },
		.icon = &st::menuIconIpAddress,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_system_dns_about,
	},
	{
		.option = &kFasterTransfers,
		.title = tr::lng_serein_faster_transfers,
		.id = u"serein/services/faster-transfers"_q,
		.keywords = { u"upload"_q, u"download"_q, u"speed"_q },
		.icon = &st::menuIconNetwork,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_faster_transfers_summary,
	},
	{
		.option = &kAndroidWebApps,
		.title = tr::lng_serein_android_web_apps,
		.id = u"serein/services/android-web-apps"_q,
		.keywords = { u"mini apps"_q, u"web apps"_q, u"bots"_q, u"Android"_q },
		.icon = &st::menuIconBotCommands,
		.tile = &st::settingsIconBg8,
		.about = tr::lng_serein_android_web_apps_summary,
	},
} };

struct CustomRows {
	CustomRow services;
	CustomRow preferSystemAi;
	CustomRow sendTranslations;
	CustomRow proxySubscription;
	CustomRow proxyNotes;
	CustomRow customDoh;
	CustomRow datacenters;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddSection(builder, {
		u"serein/services/providers"_q,
		tr::lng_serein_section_providers,
		{ u"service"_q, u"API"_q },
	});
	custom.services();
	EndSection(builder, tr::lng_serein_services_about);
	AddSection(builder, {
		u"serein/services/system-ai"_q,
		tr::lng_serein_section_system_ai,
		{ u"AI"_q, u"Apple Intelligence"_q },
	});
	custom.preferSystemAi();
	EndSection(builder, tr::lng_serein_system_ai_about);
	AddSection(builder, {
		u"serein/services/translation"_q,
		tr::lng_serein_section_translation,
		{ u"translate"_q, u"transcribe"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddNote(builder, tr::lng_serein_translation_context_about);
	AddToggle(builder, kToggleRows[1]);
	AddNote(builder, tr::lng_serein_chat_translation_about);
	AddToggle(builder, kToggleRows[2]);
	AddNote(builder, tr::lng_serein_auto_translate_chats_about);
	custom.sendTranslations();
	AddToggle(builder, kToggleRows[3]);
	AddNote(builder, tr::lng_serein_instant_view_translation_about);
	AddChoice(builder, {
		.option = &kAutoTranscribe,
		.title = tr::lng_serein_auto_transcribe,
		.id = u"serein/services/auto-transcribe"_q,
		.keywords = { u"voice"_q, u"transcribe"_q, u"speech"_q, u"automatic"_q },
		.values = { 0, 1, 2 },
		.labels = { tr::lng_serein_auto_transcribe_off, tr::lng_serein_auto_transcribe_private, tr::lng_serein_auto_transcribe_all },
		.icon = &st::menuIconVideoChat,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_auto_transcribe_summary,
	});
	EndSection(builder, tr::lng_serein_auto_transcribe_about);
	AddSection(builder, {
		u"serein/services/network"_q,
		tr::lng_serein_section_network,
		{ u"proxy"_q, u"DNS"_q, u"network"_q },
	});
	custom.proxySubscription();
	custom.proxyNotes();
	AddToggle(builder, kToggleRows[4]);
	AddNote(builder, tr::lng_serein_proxy_vpn_about);
	custom.customDoh();
	AddToggle(builder, kToggleRows[5]);
	AddNote(builder, tr::lng_serein_system_dns_note);
	AddToggle(builder, kToggleRows[6]);
	AddNote(builder, tr::lng_serein_faster_transfers_about);
	custom.datacenters();
	EndSection(builder, tr::lng_serein_dc_status_about);
	AddSection(builder, {
		u"serein/services/mini-apps"_q,
		tr::lng_serein_section_mini_apps,
		{ u"mini apps"_q, u"bots"_q },
	});
	AddToggle(builder, kToggleRows[7]);
	EndSection(builder, tr::lng_serein_android_web_apps_about);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_services;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_services_about;
inline const auto kSubpageIcon = &st::menuIconTranslate;
inline const auto kSubpageTile = &st::settingsIconBg5;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"translation"_q, u"AI"_q, u"service"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::ServiceSettings
