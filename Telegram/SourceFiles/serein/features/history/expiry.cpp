#include "serein/hooks/history.h"

#include "serein/hooks/gen/history.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

#include <algorithm>

namespace Serein::Hooks {

std::vector<gsl::not_null<HistoryItem*>> OnExpiredMessages(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	auto destroyed = OnServerDeleted(items);
	for (const auto &item : items) {
		const auto kept = std::find(
			destroyed.begin(),
			destroyed.end(),
			item) == destroyed.end();
		if (kept && item->ttlDestroyAt() > 0) {
			item->history()->owner().unregisterMessageTTL(
				item->ttlDestroyAt(),
				item);
		}
	}
	return destroyed;
}

bool KeepExpiredMedia(gsl::not_null<const HistoryItem*> item) {
	return HistorySettings::HistoryKeepExpiredMedia(
		&item->history()->session());
}

} // namespace Serein::Hooks
