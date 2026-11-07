#include "serein/features/updates/model/manifest.h"
#include "serein/features/updates/model/version.h"

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <doctest/doctest.h>

namespace Serein::Updates {
namespace {

[[nodiscard]] QJsonObject Fixture() {
	auto file = QFile(QString::fromUtf8(SEREIN_RELEASE_FIXTURE));
	REQUIRE(file.open(QIODevice::ReadOnly));
	return QJsonDocument::fromJson(file.readAll()).object();
}

[[nodiscard]] std::optional<ReleaseManifest> Parse(const QJsonObject &root) {
	return ParseManifest(QJsonDocument(root).toJson());
}

[[nodiscard]] QJsonObject WithAsset(QJsonObject root, QJsonObject asset) {
	root.insert(u"assets"_q, QJsonArray{ asset });
	return root;
}

} // namespace

TEST_CASE("UpdateManifest") {
	const auto fixture = Fixture();
	SUBCASE("the published manifest parses") {
		const auto manifest = Parse(fixture);
		REQUIRE(manifest);
		CHECK(manifest->channel == u"nightly"_q);
		CHECK(manifest->tag == u"nightly"_q);
		CHECK(manifest->version == u"7.2.10"_q);
		CHECK(manifest->commit.size() == 40);
		CHECK(manifest->assets.size() == 5);
	}
	SUBCASE("the asset follows the install") {
		const auto manifest = Parse(fixture);
		REQUIRE(manifest);
		const auto name = [&](const char *os, const char *arch, const char *kind) {
			const auto asset = ChooseAsset(*manifest, {
				QString::fromLatin1(os),
				QString::fromLatin1(arch),
				QString::fromLatin1(kind),
			});
			return asset ? asset->name : QString();
		};
		CHECK(name("macos", "arm64", "disk-image")
			== u"SereinGram-macos-arm64.dmg"_q);
		CHECK(name("macos", "x86_64", "disk-image")
			== u"SereinGram-macos-universal.dmg"_q);
		CHECK(name("windows", "x86_64", "installer")
			== u"SereinGram-windows-x86_64-setup.exe"_q);
		CHECK(name("linux", "x86_64", "appimage")
			== u"SereinGram-linux-x86_64.AppImage"_q);
		CHECK(name("windows", "arm64", "installer").isEmpty());
		CHECK(name("linux", "x86_64", "deb").isEmpty());
	}
	SUBCASE("additions are ignored and other schemas are refused") {
		auto root = fixture;
		root.insert(u"added_later"_q, QJsonObject{ { u"x"_q, 1 } });
		CHECK(Parse(root));
		root.insert(u"schema_version"_q, 2);
		CHECK(!Parse(root));
		root = fixture;
		root.insert(u"commit"_q, u"develop"_q);
		CHECK(!Parse(root));
		root = fixture;
		root.insert(u"tag"_q, u"../nightly"_q);
		CHECK(!Parse(root));
	}
	SUBCASE("unsafe assets are dropped") {
		const auto asset = fixture.value(u"assets"_q).toArray().at(0).toObject();
		const auto dropped = [&](const QString &key, const QJsonValue &value) {
			auto changed = asset;
			changed.insert(key, value);
			const auto manifest = Parse(WithAsset(fixture, changed));
			return manifest && manifest->assets.empty();
		};
		CHECK(!dropped(u"os"_q, u"windows"_q));
		CHECK(dropped(u"url"_q, u"https://example.com/SereinGram-a.exe"_q));
		CHECK(dropped(u"url"_q, u"http://github.com/o/r/SereinGram-a.exe"_q));
		CHECK(dropped(u"name"_q, u"SereinGram-other.exe"_q));
		CHECK(dropped(u"sha256"_q, u"abc"_q));
		CHECK(dropped(u"size"_q, 0));
		CHECK(dropped(u"kind"_q, u"Installer"_q));
	}
}

TEST_CASE("UpdateLinuxInstall") {
	const auto kind = [](
			const char *package,
			const char *directory,
			bool appImage,
			bool dpkg) {
		return LinuxInstallKind({
			.buildPackage = QString::fromLatin1(package),
			.directory = QString::fromLatin1(directory),
			.appImage = appImage,
			.dpkg = dpkg,
		});
	};
	CHECK(kind("snap", "/snap/sereingram/x1/usr/bin", false, true) == u"snap"_q);
	CHECK(kind("pacman", "/usr/bin", false, false) == u"pacman"_q);
	CHECK(kind("flatpak", "/app/bin", false, false) == u"flatpak"_q);
	CHECK(kind("", "/tmp/.mount_Serein/usr/bin", true, true) == u"appimage"_q);
	CHECK(kind("", "/usr/bin", false, true) == u"deb"_q);
	CHECK(kind("", "/usr/bin", false, false) == u"rpm"_q);
	CHECK(kind("", "/usr/local/bin", false, true) == u"portable"_q);
	CHECK(kind("", "/home/user/Downloads", false, false) == u"portable"_q);
}

TEST_CASE("UpdateVersions") {
	CHECK(IsNewer(u"v7.2.10.1"_q, u"7.2.10"_q));
	CHECK(IsNewer(u"7.3"_q, u"7.2.10"_q));
	CHECK(IsNewer(u"7.2.10"_q, u"7.2.10-beta"_q));
	CHECK(!IsNewer(u"7.2.10"_q, u"7.2.10"_q));
	CHECK(!IsNewer(u"7.2.10.0"_q, u"7.2.10"_q));
	CHECK(!IsNewer(u"7.2.10-beta"_q, u"7.2.10"_q));
	CHECK(!IsNewer(u"7.2.9"_q, u"7.2.10"_q));
	CHECK(!IsNewer(u"beta"_q, u"7.2.10"_q));
	CHECK(!IsNewer(u"7.3"_q, u""_q));
}

} // namespace Serein::Updates
