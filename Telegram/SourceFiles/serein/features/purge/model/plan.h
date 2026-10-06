#pragma once

#include <QtCore/QtGlobal>

namespace Serein::Purge {

enum class Age {
	All,
	Day,
	Week,
	Month,
	Year,
	Custom,
};

// Messages sent before the result are deleted, zero means every date.
[[nodiscard]] qint64 Cutoff(Age age, qint64 now, qint64 custom);

} // namespace Serein::Purge
