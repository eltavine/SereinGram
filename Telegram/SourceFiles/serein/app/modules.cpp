#include "serein/app/modules.h"

#include "serein/app/auto_demo.h"
#include "serein/app/message_menu.h"
#include "serein/app/updates.h"
#include "serein/filters/subscription.h"

#include "serein/app/reading_positions.h"
#include "serein/app/recent_chats.h"
#include "serein/app/shortcuts.h"

#include "serein/hooks/chats/sort.h"
#include "serein/hooks/history.h"
#include "serein/hooks/interface/roundness.h"
#include "serein/hooks/interface/text.h"
#include "serein/interface/app_icon.h"
#include "serein/interface/global_shortcut.h"
#include "serein/media/voice_denoise.h"
#include "serein/messages/chinese_warmup.h"
#include "serein/messages/selection_limit.h"
#include "serein/network/vpn_proxy.h"
#include "serein/settings/lock.h"

#include <array>

namespace Serein::App {
namespace {

constexpr auto kModules = std::array{
	Module{ "interface.roundness", Interface::StartRoundness },
	Module{ "interface.text", Interface::StartUiText },
	Module{ "interface.app_icon", Interface::StartAppIcon },
	Module{ "history.retention", nullptr, Hooks::PruneHistory },
	Module{ "history.removed_chats", nullptr, Hooks::WatchRemovedChats },
	Module{ "updates.check", StartUpdateChecks },
	Module{ "chats.sorting", nullptr, Chats::WatchSorting },
	Module{ "app.shortcuts", StartShortcuts },
	Module{ "app.message_menu", RegisterMessageMenu },
	Module{ "privacy.auto_demo", StartAutoDemoMode },
	Module{ "privacy.settings_lock", StartSettingsLock },
	Module{ "media.voice_denoise", Media::StartVoiceDenoise },
	Module{ "messages.selection_limit", Messages::StartSelectionLimit },
	Module{ "messages.chinese_warmup", Messages::StartChineseWarmUp },
	Module{ "interface.global_shortcut", Interface::StartGlobalShortcut },
	Module{ "network.vpn_proxy", Network::StartVpnProxyPause },
	Module{ "filters.subscription", Filters::StartRuleSubscription },
	Module{ "chats.recent", nullptr, nullptr, TrackRecentChats },
	Module{ "chats.reading_position", nullptr, nullptr, TrackReadingPositions },
};

} // namespace

std::span<const Module> Modules() {
	return kModules;
}

} // namespace Serein::App
