#pragma once

#include <QtCore/QRegularExpression>

namespace Serein {

// WHY: user patterns run without JIT and with match, depth and heap limits,
// so a pathological expression fails instead of freezing the app.
[[nodiscard]] QRegularExpression SafePattern(
	const QString &pattern,
	bool caseInsensitive);
[[nodiscard]] QRegularExpression CachedSafePattern(
	const QString &pattern,
	bool caseInsensitive);

} // namespace Serein
