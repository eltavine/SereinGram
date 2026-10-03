#include "serein/features/updates/model/manifest.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrl>

namespace Serein::Updates {
namespace {

constexpr auto kMaxAssets = 64;

[[nodiscard]] bool Matches(const QString &value, const char *pattern) {
	return QRegularExpression(QString::fromLatin1(pattern)).match(
		value).hasMatch();
}

[[nodiscard]] bool Token(const QString &value) {
	return Matches(value, "\\A[a-z0-9][a-z0-9_-]{0,31}\\z");
}

[[nodiscard]] bool GitHubUrl(const QString &value) {
	const auto url = QUrl(value, QUrl::StrictMode);
	return url.isValid()
		&& url.scheme() == u"https"_q
		&& url.host() == u"github.com"_q;
}

[[nodiscard]] std::optional<ReleaseAsset> ParseAsset(
		const QJsonObject &object) {
	auto result = ReleaseAsset{
		.name = object.value(u"name"_q).toString(),
		.os = object.value(u"os"_q).toString(),
		.arch = object.value(u"arch"_q).toString(),
		.kind = object.value(u"kind"_q).toString(),
		.url = object.value(u"url"_q).toString(),
		.sha256 = object.value(u"sha256"_q).toString(),
		.size = int64(object.value(u"size"_q).toDouble()),
	};
	if (!Matches(result.name, "\\ASereinGram-[A-Za-z0-9._-]{1,96}\\z")
		|| !Token(result.os)
		|| !Token(result.arch)
		|| !Token(result.kind)
		|| !GitHubUrl(result.url)
		|| !QUrl(result.url).path().endsWith(u'/' + result.name)
		|| !Matches(result.sha256, "\\A[0-9a-f]{64}\\z")
		|| result.size <= 0) {
		return std::nullopt;
	}
	return result;
}

} // namespace

std::optional<ReleaseManifest> ParseManifest(const QByteArray &json) {
	const auto root = QJsonDocument::fromJson(json).object();
	if (root.value(u"schema_version"_q).toInt() != kManifestSchema) {
		return std::nullopt;
	}
	auto result = ReleaseManifest{
		.channel = root.value(u"channel"_q).toString(),
		.tag = root.value(u"tag"_q).toString(),
		.version = root.value(u"version"_q).toString(),
		.commit = root.value(u"commit"_q).toString(),
	};
	if (!Token(result.channel)
		|| !Matches(result.tag, "\\A[A-Za-z0-9._-]{1,64}\\z")
		|| !Matches(result.version, "\\A[A-Za-z0-9._+-]{1,64}\\z")
		|| !Matches(result.commit, "\\A[0-9a-f]{40}\\z")) {
		return std::nullopt;
	}
	for (const auto &value : root.value(u"assets"_q).toArray()) {
		if (int(result.assets.size()) == kMaxAssets) {
			break;
		} else if (auto asset = ParseAsset(value.toObject())) {
			result.assets.push_back(std::move(*asset));
		}
	}
	return result;
}

const ReleaseAsset *ChooseAsset(
		const ReleaseManifest &manifest,
		const InstallTarget &target) {
	const auto find = [&](const QString &arch) -> const ReleaseAsset* {
		for (const auto &asset : manifest.assets) {
			if (asset.os == target.os
				&& asset.arch == arch
				&& asset.kind == target.kind) {
				return &asset;
			}
		}
		return nullptr;
	};
	if (const auto exact = find(target.arch)) {
		return exact;
	}
	return find(u"universal"_q);
}

} // namespace Serein::Updates
