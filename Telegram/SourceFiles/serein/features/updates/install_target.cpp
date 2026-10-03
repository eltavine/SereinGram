#include "serein/features/updates/install_target.h"

#include "base/platform/base_platform_info.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSysInfo>

namespace Serein::Updates {
namespace {

[[nodiscard]] QString Architecture() {
	const auto arch = QSysInfo::buildCpuArchitecture();
	return (arch == u"arm64"_q || arch == u"aarch64"_q)
		? u"arm64"_q
		: u"x86_64"_q;
}

[[nodiscard]] QString WindowsKind() {
	const auto directory = QDir(QCoreApplication::applicationDirPath());
	return QFile::exists(directory.filePath(u"unins000.exe"_q))
		? u"installer"_q
		: u"portable"_q;
}

[[nodiscard]] QString LinuxKind() {
	const auto directory = QCoreApplication::applicationDirPath();
	if (qEnvironmentVariableIsSet("APPIMAGE")) {
		return u"appimage"_q;
	} else if (QFile::exists(u"/.flatpak-info"_q)) {
		return u"flatpak"_q;
	} else if (directory.startsWith(u"/usr/"_q)) {
		return QFile::exists(u"/usr/bin/dpkg"_q) ? u"deb"_q : u"rpm"_q;
	}
	return u"portable"_q;
}

} // namespace

InstallTarget CurrentInstall() {
	if (Platform::IsWindows()) {
		return { u"windows"_q, Architecture(), WindowsKind() };
	} else if (Platform::IsMac()) {
		return { u"macos"_q, Architecture(), u"disk-image"_q };
	}
	return { u"linux"_q, Architecture(), LinuxKind() };
}

} // namespace Serein::Updates
