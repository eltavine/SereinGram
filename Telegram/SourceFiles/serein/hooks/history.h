#pragma once

#include <gsl/pointers>

#include <vector>

class HistoryItem;
struct TextWithEntities;

namespace Main {
class Session;
} // namespace Main

namespace Serein::Ports {
class HistoryStore;
} // namespace Serein::Ports

namespace Serein::Hooks {

[[nodiscard]] std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
	std::vector<gsl::not_null<HistoryItem*>> items);
void OnBeforeEdition(
	gsl::not_null<HistoryItem*> item,
	const TextWithEntities &updated);

[[nodiscard]] Ports::HistoryStore *HistoryStoreFor(
	gsl::not_null<Main::Session*> session);
void PruneHistory(gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks
