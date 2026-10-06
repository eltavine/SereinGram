#include "serein/core/options.h"
#include "serein/menu/model.h"
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
#include "serein/tests/full_registry.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <map>
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
	{ "serein.highlightHistoryMarks", "needs history recording, off by default" },
	{ "serein.historyQuoteDeletedReplies", "needs deleted messages kept in chat, off by default" },
};

const auto kGatedMenuActions = std::map<Serein::Menu::ActionId, std::string_view>{
	{ Serein::Menu::ActionId::EditHistory, "needs history recording, off by default" },
	{ Serein::Menu::ActionId::DeletedMessages, "needs history recording, off by default" },
	{ Serein::Menu::ActionId::RestoredMedia, "only in the history viewer, which needs recording" },
	{ Serein::Menu::ActionId::HistoryExclusion, "needs history recording, off by default" },
	{ Serein::Menu::ActionId::ReadUntilHere, "needs ghost read receipts, off by default" },
	{ Serein::Menu::ActionId::QuickRatingFirst, "needs a quick rating, unset by default" },
	{ Serein::Menu::ActionId::QuickRatingSecond, "needs a quick rating, unset by default" },
};

void RequireKey(bool value, const char *message, std::string_view key) {
	INFO((std::string("option ") + std::string(key)));
	::Require(value, message);
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

TEST_CASE("NeutralDefaults") {
	using namespace Serein;
	const auto registry = Tests::FullRegistry();
	auto inert = std::size_t();
	for (const auto &entry : registry.All()) {
		const auto listed = kInertDefaults.contains(entry.key);
		if (Neutral(entry)) {
			RequireKey(!listed, "listed default is already neutral", entry.key);
		} else {
			RequireKey(listed, "default is not off, zero or empty", entry.key);
			++inert;
		}
	}
	RequireKey(inert == kInertDefaults.size(), "unknown listed default", {});
	for (const auto &entry : Menu::kEntries) {
		const auto shown = (Menu::DefaultVisibility(entry.id)
			== Menu::Visibility::Show);
		if (int(entry.id) < int(Menu::ActionId::Repeat)) {
			RequireKey(shown, "upstream menu action hidden", entry.titleKey);
		} else if (kGatedMenuActions.contains(entry.id)) {
			RequireKey(shown, "listed menu action is already hidden", entry.titleKey);
		} else {
			RequireKey(!shown, "Serein menu action shown by default", entry.titleKey);
		}
	}
}
