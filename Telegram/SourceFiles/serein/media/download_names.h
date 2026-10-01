#pragma once

#include <QtCore/QString>

namespace Serein::Media {

[[nodiscard]] QString DownloadFolderName(const QString &name, quint64 id);

} // namespace Serein::Media
