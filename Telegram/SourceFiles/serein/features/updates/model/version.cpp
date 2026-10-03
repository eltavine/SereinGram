#include "serein/features/updates/model/version.h"

#include <QtCore/QVersionNumber>

#include <optional>

namespace Serein::Updates {
namespace {

struct Version {
	QVersionNumber number;
	bool prerelease = false;
};

[[nodiscard]] std::optional<Version> Parse(QString value) {
	if (value.startsWith(u'v') || value.startsWith(u'V')) {
		value.remove(0, 1);
	}
	auto suffix = qsizetype(0);
	const auto number = QVersionNumber::fromString(value, &suffix);
	if (number.isNull()) {
		return std::nullopt;
	}
	return Version{ number.normalized(), suffix < value.size() };
}

} // namespace

bool IsNewer(const QString &candidate, const QString &current) {
	const auto left = Parse(candidate);
	const auto right = Parse(current);
	if (!left || !right) {
		return false;
	}
	const auto order = QVersionNumber::compare(left->number, right->number);
	return (order > 0)
		|| (order == 0 && right->prerelease && !left->prerelease);
}

} // namespace Serein::Updates
