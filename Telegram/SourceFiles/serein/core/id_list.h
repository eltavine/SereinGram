#pragma once

#include <QtCore/QString>

#include <optional>
#include <vector>

namespace Serein {

[[nodiscard]] std::optional<std::vector<quint64>> ParseIdList(
	const QString &value,
	int limit);

} // namespace Serein
