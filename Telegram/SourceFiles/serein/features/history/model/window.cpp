#include "serein/features/history/model/window.h"

#include <algorithm>

namespace Serein::HistoryFeature {

RecordsWindow LoadWindow(
		Ports::HistoryStore &store,
		Ports::RecordsQuery filter,
		const WindowRequest &request) {
	filter.from = filter.till = std::nullopt;
	filter.limit = std::nullopt;

	auto older = filter;
	older.order = Ports::RecordsOrder::Descending;
	auto newer = filter;
	newer.order = Ports::RecordsOrder::Ascending;
	const auto before = std::max(request.limitBefore, 0);
	const auto after = std::max(request.limitAfter, 0);
	if (const auto around = request.around) {
		older.till = Ports::RecordBound{ .key = *around };
		older.limit = before;
		newer.from = Ports::RecordBound{ .key = *around, .inclusive = true };
		newer.limit = after + 1;
	} else {
		older.limit = before + 1;
		newer.limit = 0;
	}

	auto result = RecordsWindow();
	if (*older.limit > 0) {
		result.records = store.records(older);
		std::reverse(begin(result.records), end(result.records));
	}
	if (*newer.limit > 0) {
		auto following = store.records(newer);
		result.nearest = following.empty()
			? std::optional<Ports::RecordKey>()
			: Ports::KeyOf(following.front());
		result.records.insert(
			end(result.records),
			std::make_move_iterator(begin(following)),
			std::make_move_iterator(end(following)));
	}
	if (result.records.empty()) {
		result.skippedBefore = store.count(filter);
		return result;
	} else if (!result.nearest) {
		result.nearest = Ports::KeyOf(result.records.back());
	}
	auto preceding = filter;
	preceding.till = Ports::RecordBound{
		.key = Ports::KeyOf(result.records.front()),
	};
	auto following = filter;
	following.from = Ports::RecordBound{
		.key = Ports::KeyOf(result.records.back()),
	};
	result.skippedBefore = store.count(preceding);
	result.skippedAfter = store.count(following);
	return result;
}

} // namespace Serein::HistoryFeature
