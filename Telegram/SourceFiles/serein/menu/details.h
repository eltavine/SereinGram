#pragma once

#include <QtCore/QString>

#include <gsl/pointers>

#include <utility>
#include <vector>

class HistoryItem;

namespace Serein::Menu {

[[nodiscard]] std::vector<std::pair<QString, QString>> StickerFacts(
	gsl::not_null<HistoryItem*> item);

} // namespace Serein::Menu
