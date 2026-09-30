#pragma once

#include <QtCore/QString>

#include <vector>

namespace Serein::Privacy {

[[nodiscard]] bool IsRecorderProcess(const QString &name);
[[nodiscard]] bool RecorderRunning(const std::vector<QString> &names);
[[nodiscard]] std::vector<QString> RunningProcessNames();

} // namespace Serein::Privacy
