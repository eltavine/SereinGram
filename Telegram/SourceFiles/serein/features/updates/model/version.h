#pragma once

#include <QtCore/QString>

namespace Serein::Updates {

// A suffix after the numbers, as in 7.3-beta, marks a pre-release.
[[nodiscard]] bool IsNewer(const QString &candidate, const QString &current);

} // namespace Serein::Updates
