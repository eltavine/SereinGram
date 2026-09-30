#pragma once

#include <gsl/pointers>

#include <vector>

class HistoryItem;

namespace Serein::HistoryFeature {

[[nodiscard]] std::vector<gsl::not_null<HistoryItem*>> KeepDeletedInPlace(
	std::vector<gsl::not_null<HistoryItem*>> items);
[[nodiscard]] bool DeletedInPlace(gsl::not_null<const HistoryItem*> item);

} // namespace Serein::HistoryFeature
