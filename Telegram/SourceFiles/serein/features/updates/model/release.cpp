#include "serein/features/updates/model/release.h"

#include "base/basic_types.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrl>

#include <algorithm>

namespace Serein::Updates {
namespace {

[[nodiscard]] std::optional<QString> GitHubPage(const QJsonObject &release) {
	const auto url = QUrl(
		release.value(u"html_url"_q).toString(),
		QUrl::StrictMode);
	if (!url.isValid()
		|| url.scheme() != u"https"_q
		|| url.host() != u"github.com"_q) {
		return std::nullopt;
	}
	return url.toString(QUrl::FullyEncoded);
}

} // namespace

std::optional<Release> ParseLatestRelease(const QByteArray &json) {
	const auto root = QJsonDocument::fromJson(json).object();
	const auto tag = root.value(u"tag_name"_q).toString();
	const auto url = GitHubPage(root);
	if (VersionParts(tag).empty()
		|| root.value(u"draft"_q).toBool()
		|| root.value(u"prerelease"_q).toBool()
		|| !url) {
		return std::nullopt;
	}
	return Release{ tag, *url };
}

std::optional<Nightly> ParseNightlyRelease(const QByteArray &json) {
	static const auto hash = QRegularExpression(u"\\A[0-9a-f]{40}\\z"_q);
	const auto root = QJsonDocument::fromJson(json).object();
	const auto commit = root.value(u"target_commitish"_q).toString();
	const auto url = GitHubPage(root);
	if (root.value(u"tag_name"_q).toString() != u"nightly"_q
		|| root.value(u"draft"_q).toBool()
		|| !hash.match(commit).hasMatch()
		|| !url) {
		return std::nullopt;
	}
	return Nightly{ commit, *url };
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
