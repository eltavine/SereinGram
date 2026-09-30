#include "serein/features/updates/model/release.h"

#include "base/basic_types.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrl>

#include <algorithm>

namespace Serein::Updates {

std::optional<Release> ParseLatestRelease(const QByteArray &json) {
	const auto root = QJsonDocument::fromJson(json).object();
	const auto tag = root.value(u"tag_name"_q).toString();
	const auto url = QUrl(root.value(u"html_url"_q).toString(), QUrl::StrictMode);
	if (VersionParts(tag).empty()
		|| root.value(u"draft"_q).toBool()
		|| root.value(u"prerelease"_q).toBool()
		|| !url.isValid()
		|| url.scheme() != u"https"_q
		|| url.host() != u"github.com"_q) {
		return std::nullopt;
	}
	return Release{ tag, url.toString(QUrl::FullyEncoded) };
}

std::vector<int> VersionParts(const QString &version) {
	static const auto leading = QRegularExpression(
		u"\\Av?(\\d{1,9}(?:\\.\\d{1,9})*)"_q);
	const auto match = leading.match(version);
	auto result = std::vector<int>();
	if (!match.hasMatch()) {
		return result;
	}
	for (const auto &part : match.captured(1).split('.')) {
		result.push_back(part.toInt());
	}
	return result;
}

bool IsNewer(const QString &candidate, const QString &current) {
	auto left = VersionParts(candidate);
	auto right = VersionParts(current);
	if (left.empty() || right.empty()) {
		return false;
	}
	const auto size = std::max(left.size(), right.size());
	left.resize(size);
	right.resize(size);
	return left > right;
}

} // namespace Serein::Updates
