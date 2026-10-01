#include "serein/hooks/chats/managed_folders.h"

#include "serein/hooks/gen/chats.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/chats.h"
#include "base/flat_set.h"
#include "data/data_chat_filters.h"
#include "data/data_premium_limits.h"
#include "data/data_session.h"
#include "main/main_session.h"

namespace Serein::Chats {
namespace {

[[nodiscard]] int FiltersLimit(not_null<Main::Session*> session) {
	return 1 + Data::PremiumLimits(session).dialogFiltersCurrent();
}

[[nodiscard]] base::flat_set<FilterId> HiddenFolders(
		not_null<Main::Session*> session) {
	auto result = base::flat_set<FilterId>();
	const auto value = ForAccount(session).Get(kHiddenFolderIds);
	for (const auto &part : value.split(u',', Qt::SkipEmptyParts)) {
		result.emplace(FilterId(part.toInt()));
	}
	return result;
}

[[nodiscard]] bool Shown(
		const Data::ChatFilter &filter,
		bool allChatsHidden,
		const base::flat_set<FilterId> &hidden) {
	return filter.id()
		? !hidden.contains(filter.id())
		: !allChatsHidden;
}

} // namespace

bool AllChatsHidden(not_null<Main::Session*> session) {
	if (!Hooks::Chats::HideAllChatsFolder()) {
		return false;
	}
	const auto hidden = HiddenFolders(session);
	const auto &list = session->data().chatsFilters().list();
	const auto limit = std::min(int(list.size()), FiltersLimit(session));
	for (auto i = 0; i != limit; ++i) {
		if (list[i].id() && !hidden.contains(list[i].id())) {
			return true;
		}
	}
	return false;
}

std::vector<Data::ChatFilter> ShownFilters(not_null<Main::Session*> session) {
	const auto allChatsHidden = AllChatsHidden(session);
	const auto hidden = HiddenFolders(session);
	return session->data().chatsFilters().list()
		| ranges::views::filter([&](const Data::ChatFilter &filter) {
			return Shown(filter, allChatsHidden, hidden);
		}) | ranges::to_vector;
}

int ShownFiltersLimit(not_null<Main::Session*> session) {
	const auto &list = session->data().chatsFilters().list();
	const auto limit = FiltersLimit(session);
	const auto allChatsHidden = AllChatsHidden(session);
	const auto hidden = HiddenFolders(session);
	const auto checked = std::min(int(list.size()), limit);
	auto hiddenInsideLimit = 0;
	for (auto i = 0; i != checked; ++i) {
		hiddenInsideLimit += Shown(list[i], allChatsHidden, hidden) ? 0 : 1;
	}
	return limit - hiddenInsideLimit;
}

void SaveShownOrder(
		not_null<Main::Session*> session,
		const std::vector<FilterId> &order) {
	auto &filters = session->data().chatsFilters();
	const auto allChatsHidden = AllChatsHidden(session);
	const auto hidden = HiddenFolders(session);
	if (!allChatsHidden && hidden.empty()) {
		filters.saveOrder(order);
		return;
	}
	Expects(order.size() == ShownFilters(session).size());
	auto full = std::vector<FilterId>();
	full.reserve(filters.list().size());
	auto i = 0;
	for (const auto &filter : filters.list()) {
		full.push_back(Shown(filter, allChatsHidden, hidden)
			? order[i++]
			: filter.id());
	}
	filters.saveOrder(full);
}

rpl::producer<> ShownFiltersChanges(not_null<Main::Session*> session) {
	return rpl::combine(
		Hooks::Chats::HideAllChatsFolderValue(),
		ForAccount(session).Value(kHiddenFolderIds)
	) | rpl::to_empty;
}

} // namespace Serein::Chats
