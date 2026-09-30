#pragma once

#include "serein/filters/model.h"

class HistoryItem;

namespace Serein::Filters {

[[nodiscard]] Result Project(
	HistoryItem *item,
	const TextWithEntities &source);
[[nodiscard]] bool Hidden(HistoryItem *item);
[[nodiscard]] TextWithEntities DisplayText(const Result &result);

} // namespace Serein::Filters
