#pragma once

#include <QtCore/QString>

#include <vector>

namespace Serein::Chats {

inline constexpr auto kLocalPinsLimit = 50;

[[nodiscard]] std::vector<quint64> ParseLocalPins(const QString &value);
[[nodiscard]] QString ToggleLocalPin(const QString &value, quint64 id);

} // namespace Serein::Chats
