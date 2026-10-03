#pragma once

#include "base/basic_types.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <optional>
#include <vector>

namespace Serein::Updates {

struct ReleaseAsset {
	QString name;
	QString os;
	QString arch;
	QString kind;
	QString url;
	QString sha256;
	int64 size = 0;
};

struct ReleaseManifest {
	QString channel;
	QString tag;
	QString version;
	QString commit;
	std::vector<ReleaseAsset> assets;
};

struct InstallTarget {
	QString os;
	QString arch;
	QString kind;
};

struct LinuxInstall {
	QString buildPackage;
	QString directory;
	bool appImage = false;
	bool dpkg = false;
};

inline constexpr auto kManifestSchema = 1;

[[nodiscard]] std::optional<ReleaseManifest> ParseManifest(
	const QByteArray &json);
[[nodiscard]] const ReleaseAsset *ChooseAsset(
	const ReleaseManifest &manifest,
	const InstallTarget &target);
[[nodiscard]] QString LinuxInstallKind(const LinuxInstall &install);

} // namespace Serein::Updates
