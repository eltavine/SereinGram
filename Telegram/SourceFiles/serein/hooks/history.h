#pragma once

#include <gsl/pointers>

#include <vector>

class HistoryItem;
struct TextWithEntities;

namespace Serein::Hooks {

void OnServerDeleted(const std::vector<gsl::not_null<HistoryItem*>> &items);
void OnBeforeEdition(
	gsl::not_null<HistoryItem*> item,
	const TextWithEntities &updated);

} // namespace Serein::Hooks
