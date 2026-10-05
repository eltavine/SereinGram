#pragma once

#include "serein/ports/history_store.h"

#include <optional>
#include <vector>

namespace Serein::HistoryFeature {

struct WindowRequest {
	std::optional<Ports::RecordKey> around;
	int limitBefore = 0;
	int limitAfter = 0;
};

struct RecordsWindow {
	std::vector<History::Record> records;
	std::optional<Ports::RecordKey> nearest;
	int skippedBefore = 0;
	int skippedAfter = 0;

	[[nodiscard]] int fullCount() const {
		return skippedBefore + int(records.size()) + skippedAfter;
	}
};

// Without an anchor the window ends at the newest matching record.
[[nodiscard]] RecordsWindow LoadWindow(
	Ports::HistoryStore &store,
	Ports::RecordsQuery filter,
	const WindowRequest &request);

} // namespace Serein::HistoryFeature
