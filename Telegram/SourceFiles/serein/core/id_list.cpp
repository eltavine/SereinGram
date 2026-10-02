#include "serein/core/id_list.h"

#include <QtCore/QStringList>

#include <algorithm>

namespace Serein {

std::optional<std::vector<quint64>> ParseIdList(
		const QString &value,
		int limit) {
	auto result = std::vector<quint64>();
	if (value.isEmpty()) {
		return result;
	}
	const auto parts = value.split(u',');
	if (parts.size() > limit) {
		return std::nullopt;
	}
	result.reserve(parts.size());
	for (const auto &part : parts) {
		auto valid = false;
		const auto id = part.toULongLong(&valid);
		if (!valid
			|| !id
			|| part != QString::number(id)
			|| std::find(result.begin(), result.end(), id) != result.end()) {
			return std::nullopt;
		}
		result.push_back(id);
	}
	return result;
}

} // namespace Serein
