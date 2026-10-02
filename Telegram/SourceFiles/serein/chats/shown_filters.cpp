#include "serein/hooks/chats/managed_folders.h"

#include "serein/hooks/gen/chats.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/chats/shown_order.h"
#include "data/data_chat_filters.h"
#include "data/data_premium_limits.h"
#include "data/data_session.h"
#include "main/main_session.h"

namespace Serein::Chats {
namespace {

[[nodiscard]] int FiltersLimit(not_null<Main::Session*> session) {
	return 1 + Data::PremiumLimits(session).dialogFiltersCurrent();
}

[[nodiscard]] FolderVisibility Visibility(
		not_null<Main::Session*> session,
		bool allChatsHidden) {
	return {
		.allChatsHidden = allChatsHidden,
		.hidden = ParseFolderIds(ForAccount(session).Get(kHiddenFolderIds)),
	};
}

[[nodiscard]] std::vector<int> Ids(const std::vector<Data::ChatFilter> &list) {
	return list | ranges::views::transform(&Data::ChatFilter::id)
		| ranges::to_vector;
}

} // namespace

bool AllChatsHidden(not_null<Main::Session*> session) {
	if (!Hooks::Chats::HideAllChatsFolder()) {
		return false;
	}
	const auto visibility = Visibility(session, false);
	const auto &list = session->data().chatsFilters().list();
	const auto limit = std::min(int(list.size()), FiltersLimit(session));
	for (auto i = 0; i != limit; ++i) {
		if (list[i].id() && visibility.shown(list[i].id())) {
			return true;
		}
	}
	return false;
}

std::vector<Data::ChatFilter> ShownFilters(not_null<Main::Session*> session) {
	const auto visibility = Visibility(session, AllChatsHidden(session));
	return session->data().chatsFilters().list()
		| ranges::views::filter([&](const Data::ChatFilter &filter) {
			return visibility.shown(filter.id());
		}) | ranges::to_vector;
}

int ShownFiltersLimit(not_null<Main::Session*> session) {
	const auto limit = FiltersLimit(session);
	return limit - HiddenWithinLimit(
		Ids(session->data().chatsFilters().list()),
		limit,
		Visibility(session, AllChatsHidden(session)));
}

void SaveShownOrder(
		not_null<Main::Session*> session,
		const std::vector<FilterId> &order) {
	auto &filters = session->data().chatsFilters();
	const auto visibility = Visibility(session, AllChatsHidden(session));
	if (!visibility.allChatsHidden && visibility.hidden.empty()) {
		filters.saveOrder(order);
		return;
	}
	Expects(order.size() == ShownFilters(session).size());
	filters.saveOrder(MergeShownOrder(Ids(filters.list()), order, visibility));
}

rpl::producer<> ShownFiltersChanges(not_null<Main::Session*> session) {
	return rpl::combine(
		Hooks::Chats::HideAllChatsFolderValue(),
		ForAccount(session).Value(kHiddenFolderIds)
	) | rpl::to_empty;
}

} // namespace Serein::Chats
