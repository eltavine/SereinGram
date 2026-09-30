#pragma once

class HistoryItem;
struct TextWithEntities;

namespace Serein::Hooks::Filters {

[[nodiscard]] bool Hidden(HistoryItem *item);
[[nodiscard]] TextWithEntities DisplayText(
	HistoryItem *item,
	const TextWithEntities &source);

} // namespace Serein::Hooks::Filters
