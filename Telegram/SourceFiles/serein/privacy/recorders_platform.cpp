#include "serein/privacy/recorders.h"

#include "base/basic_types.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#elif defined Q_OS_MAC // Q_OS_WIN
#include <libproc.h>
#include <sys/param.h>
#endif // Q_OS_WIN || Q_OS_MAC

namespace Serein::Privacy {

std::vector<QString> RunningProcessNames() {
	auto result = std::vector<QString>();
#ifdef Q_OS_WIN
	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return result;
	}
	auto entry = PROCESSENTRY32W();
	entry.dwSize = sizeof(entry);
	for (auto ok = Process32FirstW(snapshot, &entry);
		ok;
		ok = Process32NextW(snapshot, &entry)) {
		result.push_back(QString::fromWCharArray(entry.szExeFile));
	}
	CloseHandle(snapshot);
#elif defined Q_OS_MAC // Q_OS_WIN
	const auto count = proc_listallpids(nullptr, 0);
	if (count <= 0) {
		return result;
	}
	auto pids = std::vector<pid_t>(count * 2);
	const auto filled = proc_listallpids(
		pids.data(),
		int(pids.size() * sizeof(pid_t)));
	for (auto i = 0; i < filled; ++i) {
		char name[2 * MAXCOMLEN + 1] = { 0 };
		if (proc_name(pids[i], name, sizeof(name)) > 0) {
			result.push_back(QString::fromUtf8(name));
		}
	}
#else // Q_OS_WIN || Q_OS_MAC
	const auto entries = QDir(u"/proc"_q).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const auto &entry : entries) {
		auto ok = false;
		const auto pid = entry.toUInt(&ok);
		if (!ok || !pid) {
			continue;
		}
		auto file = QFile(u"/proc/"_q + entry + u"/comm"_q);
		if (file.open(QIODevice::ReadOnly)) {
			result.push_back(QString::fromUtf8(file.readAll()).trimmed());
		}
	}
#endif // Q_OS_WIN || Q_OS_MAC
	return result;
}

} // namespace Serein::Privacy
