#include "serein/core/options.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/schema/gen/settings/compose.h"
#include "serein/schema/gen/settings/filters.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/schema/gen/settings/links.h"
#include "serein/schema/gen/settings/media.h"
#include "serein/schema/gen/settings/menu.h"
#include "serein/schema/gen/settings/messages.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/schema/gen/settings/snapshot.h"

#include <map>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

const auto kInertDefaults = std::map<std::string_view, std::string_view>{
	{ "serein.quickReplies", "two empty reply slots show nothing" },
	{ "serein.ghostHideReadReceipts", "inert while ghost mode is off" },
	{ "serein.ghostHideStoryViews", "inert while ghost mode is off" },
	{ "serein.ghostHideOnline", "inert while ghost mode is off" },
	{ "serein.ghostHideTyping", "inert while ghost mode is off" },
	{ "serein.checkUpdates", "replaces the updater that builds disable" },
	{ "serein.stickerScale", "100 percent is the upstream size" },
	{ "serein.fadeDeletedMessages", "needs history recording, off by default" },
};

void Require(bool value, const char *message, std::string_view key) {
	if (!value) {
		throw std::runtime_error(std::string(message) + ": " + std::string(key));
	}
}

[[nodiscard]] bool Neutral(const Serein::OptionInfo &entry) {
	using Type = Serein::OptionInfo::ValueType;
	switch (entry.type) {
	case Type::Boolean:
	case Type::Integer: return entry.fallbackRaw == "0";
	case Type::String: return entry.fallbackRaw == "s";
	case Type::Object: return entry.fallbackRaw.isEmpty();
	}
	return false;
}

} // namespace

void TestNeutralDefaults() {
	using namespace Serein;
	auto registry = Registry();
	Chats::RegisterOptions(registry);
	Compose::RegisterOptions(registry);
	Filters::RegisterOptions(registry);
	Ghost::RegisterOptions(registry);
	HistorySettings::RegisterOptions(registry);
	Interface::RegisterOptions(registry);
	Links::RegisterOptions(registry);
	Media::RegisterOptions(registry);
	Menu::RegisterOptions(registry);
	Messages::RegisterOptions(registry);
	Privacy::RegisterOptions(registry);
	ServiceSettings::RegisterOptions(registry);
	Snapshot::RegisterOptions(registry);
	auto inert = std::size_t();
	for (const auto &entry : registry.All()) {
		const auto listed = kInertDefaults.contains(entry.key);
		if (Neutral(entry)) {
			Require(!listed, "listed default is already neutral", entry.key);
		} else {
			Require(listed, "default is not off, zero or empty", entry.key);
			++inert;
		}
	}
	Require(inert == kInertDefaults.size(), "unknown listed default", {});
}
