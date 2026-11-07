#pragma once

#include "base/basic_types.h"

#include <QtCore/QString>

namespace Data {
class LastseenStatus;
} // namespace Data

namespace Serein::Online {

[[nodiscard]] QString CompactText(
	const Data::LastseenStatus &status,
	TimeId now);
[[nodiscard]] QString ExactText(
	const Data::LastseenStatus &status,
	TimeId now,
	bool seconds);
[[nodiscard]] QString LastSeenLine(
	const Data::LastseenStatus &status,
	TimeId now,
	bool seconds);

} // namespace Serein::Online
