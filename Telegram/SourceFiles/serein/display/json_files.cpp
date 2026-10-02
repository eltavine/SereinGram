#include "serein/display/json_files.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "lang/lang_keys.h"

#include <QtCore/QFile>
#include <QtCore/QSaveFile>

namespace Serein::Display {

void SaveJsonFile(
		const QString &name,
		QByteArray bytes,
		Fn<void(QString text)> notify) {
	FileDialog::GetWritePath(
		Core::App().getFileDialogParent(),
		tr::lng_serein_config_export(tr::now),
		tr::lng_serein_config_file_filter(tr::now),
		name,
		[=](QString &&path) {
			if (path.isEmpty()) {
				return;
			}
			auto file = QSaveFile(path);
			const auto written = file.open(QIODevice::WriteOnly)
				&& (file.write(bytes) == bytes.size())
				&& file.commit();
			notify(written
				? tr::lng_serein_config_exported(tr::now)
				: tr::lng_serein_config_write_error(tr::now));
		});
}

void OpenJsonFile(
		qint64 maximumBytes,
		Fn<void(QByteArray bytes)> done,
		Fn<void(QString text)> notify) {
	FileDialog::GetOpenPath(
		Core::App().getFileDialogParent(),
		tr::lng_serein_config_import(tr::now),
		tr::lng_serein_config_file_filter(tr::now),
		[=](FileDialog::OpenResult &&result) {
			if (!result.paths.isEmpty()) {
				auto file = QFile(result.paths.front());
				if (!file.open(QIODevice::ReadOnly)) {
					notify(tr::lng_serein_config_read_error(tr::now));
					return;
				}
				done(file.read(maximumBytes + 1));
			} else if (!result.remoteContent.isEmpty()) {
				done(result.remoteContent);
			}
		});
}

} // namespace Serein::Display
