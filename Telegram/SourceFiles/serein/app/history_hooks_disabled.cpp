#include "serein/hooks/history.h"

namespace Serein::Hooks {

void OnServerDeleted(const std::vector<gsl::not_null<HistoryItem*>> &) {
}

void OnBeforeEdition(gsl::not_null<HistoryItem*>, const TextWithEntities &) {
}

Ports::HistoryStore *HistoryStoreFor(gsl::not_null<Main::Session*>) {
	return nullptr;
}

} // namespace Serein::Hooks
