#include "serein/hooks/history.h"

#include "serein/features/history/deleted_marks.h"

namespace Serein::Hooks {

std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	return HistoryFeature::KeepDeletedInPlace(std::move(items));
}

void OnBeforeEdition(gsl::not_null<HistoryItem*>, const TextWithEntities &) {
}

Ports::HistoryStore *HistoryStoreFor(gsl::not_null<Main::Session*>) {
	return nullptr;
}

} // namespace Serein::Hooks
