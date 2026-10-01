#pragma once

#include "base/flat_set.h"

#include <QtCore/QString>

#include <vector>

namespace Serein::Chats {

struct FolderVisibility {
	bool allChatsHidden = false;
	base::flat_set<int> hidden;

	[[nodiscard]] bool shown(int id) const {
		return id ? !hidden.contains(id) : !allChatsHidden;
	}
};

[[nodiscard]] base::flat_set<int> ParseFolderIds(const QString &value);
[[nodiscard]] int HiddenWithinLimit(
	const std::vector<int> &ids,
	int limit,
	const FolderVisibility &visibility);
[[nodiscard]] std::vector<int> MergeShownOrder(
	const std::vector<int> &ids,
	const std::vector<int> &shownOrder,
	const FolderVisibility &visibility);

} // namespace Serein::Chats
