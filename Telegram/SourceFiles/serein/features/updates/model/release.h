#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <optional>
#include <vector>

namespace Serein::Updates {

struct Release {
	QString tag;
	QString url;
};

[[nodiscard]] std::optional<Release> ParseLatestRelease(const QByteArray &json);
[[nodiscard]] std::vector<int> VersionParts(const QString &version);
[[nodiscard]] bool IsNewer(const QString &candidate, const QString &current);

} // namespace Serein::Updates
