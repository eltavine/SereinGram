#include "serein/chats/shown_order.h"

#include <QtCore/QStringList>

#include <algorithm>

namespace Serein::Chats {

base::flat_set<int> ParseFolderIds(const QString &value) {
	auto result = base::flat_set<int>();
	for (const auto &part : value.split(u',', Qt::SkipEmptyParts)) {
		auto ok = false;
		const auto id = part.toInt(&ok);
		if (ok && id > 0) {
			result.emplace(id);
		}
	}
	return result;
}

int HiddenWithinLimit(
		const std::vector<int> &ids,
		int limit,
		const FolderVisibility &visibility) {
	const auto checked = std::min(int(ids.size()), limit);
	return int(std::count_if(ids.begin(), ids.begin() + checked, [&](int id) {
		return !visibility.shown(id);
	}));
}

std::vector<int> MergeShownOrder(
		const std::vector<int> &ids,
		const std::vector<int> &shownOrder,
		const FolderVisibility &visibility) {
	auto result = std::vector<int>();
	result.reserve(ids.size());
	auto next = shownOrder.begin();
	for (const auto id : ids) {
		if (visibility.shown(id) && next != shownOrder.end()) {
			result.push_back(*next++);
		} else {
			result.push_back(id);
		}
	}
	return result;
}

} // namespace Serein::Chats
