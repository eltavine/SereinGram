#include "serein/hooks/chats/managed_folders.h"

#include "serein/hooks/gen/chats.h"
#include "data/data_chat_filters.h"
#include "data/data_premium_limits.h"
#include "data/data_session.h"
#include "main/main_session.h"

namespace Serein::Chats {
namespace {

[[nodiscard]] int FiltersLimit(not_null<Main::Session*> session) {
	return 1 + Data::PremiumLimits(session).dialogFiltersCurrent();
}

} // namespace

bool AllChatsHidden(not_null<Main::Session*> session) {
	if (!Hooks::Chats::HideAllChatsFolder()) {
		return false;
	}
	const auto &list = session->data().chatsFilters().list();
	const auto limit = std::min(int(list.size()), FiltersLimit(session));
	for (auto i = 0; i != limit; ++i) {
		if (list[i].id()) {
			return true;
		}
	}
	return false;
}

std::vector<Data::ChatFilter> ShownFilters(not_null<Main::Session*> session) {
	const auto hidden = AllChatsHidden(session);
	return session->data().chatsFilters().list()
		| ranges::views::filter([=](const Data::ChatFilter &filter) {
			return !hidden || filter.id();
		}) | ranges::to_vector;
}

int ShownFiltersLimit(not_null<Main::Session*> session) {
	const auto &list = session->data().chatsFilters().list();
	const auto limit = FiltersLimit(session);
	const auto all = ranges::find(list, FilterId(0), &Data::ChatFilter::id);
	const auto hiddenInsideLimit = AllChatsHidden(session)
		&& (all != end(list))
		&& ((all - begin(list)) < limit);
	return limit - (hiddenInsideLimit ? 1 : 0);
}

void SaveShownOrder(
		not_null<Main::Session*> session,
		const std::vector<FilterId> &order) {
	auto &filters = session->data().chatsFilters();
	if (!AllChatsHidden(session)) {
		filters.saveOrder(order);
		return;
	}
	Expects(order.size() == ShownFilters(session).size());
	auto full = std::vector<FilterId>();
	full.reserve(filters.list().size());
	auto i = 0;
	for (const auto &filter : filters.list()) {
		full.push_back(filter.id() ? order[i++] : FilterId(0));
	}
	filters.saveOrder(full);
}

} // namespace Serein::Chats
