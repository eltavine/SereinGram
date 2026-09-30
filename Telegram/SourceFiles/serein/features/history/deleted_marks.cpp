#include "serein/features/history/deleted_marks.h"

#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"
#include "serein/hooks/gen/history.h"

#include <map>
#include <set>

namespace Serein::HistoryFeature {
namespace {

[[nodiscard]] std::map<Main::Session*, std::set<FullMsgId>> &Marks() {
	static auto result = std::map<Main::Session*, std::set<FullMsgId>>();
	return result;
}

} // namespace

std::vector<gsl::not_null<HistoryItem*>> KeepDeletedInPlace(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	auto destroy = std::vector<gsl::not_null<HistoryItem*>>();
	for (const auto &item : items) {
		const auto session = &item->history()->session();
		if (!Hooks::HistorySettings::HistoryKeepDeletedInPlace(session)) {
			destroy.push_back(item);
			continue;
		}
		MarkDeletedInPlace(item);
		session->data().requestItemViewRefresh(item);
	}
	return destroy;
}

void MarkDeletedInPlace(gsl::not_null<HistoryItem*> item) {
	const auto session = &item->history()->session();
	const auto [i, fresh] = Marks().try_emplace(session);
	if (fresh) {
		session->lifetime().add([=] { Marks().erase(session); });
	}
	i->second.emplace(item->fullId());
}

bool DeletedInPlace(gsl::not_null<const HistoryItem*> item) {
	const auto &marks = Marks();
	const auto i = marks.find(&item->history()->session());
	return (i != marks.end()) && i->second.contains(item->fullId());
}

} // namespace Serein::HistoryFeature
