#include "serein/features/history/copies.h"

#include <unordered_map>

namespace Serein::HistoryFeature {
namespace {

using Copies = std::unordered_map<const HistoryItem*, CopyInfo>;

[[nodiscard]] Copies &All() {
	static auto result = Copies();
	return result;
}

} // namespace

void RegisterCopy(gsl::not_null<const HistoryItem*> item, CopyInfo info) {
	All().insert_or_assign(item.get(), std::move(info));
}

void UnregisterCopy(gsl::not_null<const HistoryItem*> item) {
	All().erase(item.get());
}

const CopyInfo *FindCopy(gsl::not_null<const HistoryItem*> item) {
	const auto &all = All();
	const auto i = all.find(item.get());
	return (i != end(all)) ? &i->second : nullptr;
}

} // namespace Serein::HistoryFeature
