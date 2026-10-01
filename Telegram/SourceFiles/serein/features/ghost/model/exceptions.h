#pragma once

#include <QtCore/QString>

#include <vector>

namespace Serein::Ghost {

inline constexpr auto kReadExceptionsLimit = 200;

[[nodiscard]] std::vector<quint64> ParseExceptions(const QString &value);
[[nodiscard]] bool HasException(const QString &value, quint64 peerId);
[[nodiscard]] QString ToggleException(const QString &value, quint64 peerId);

} // namespace Serein::Ghost
