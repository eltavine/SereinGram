#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <optional>

namespace Serein::Privacy {

[[nodiscard]] std::optional<QByteArray> ParseLoginToken(const QString &text);

} // namespace Serein::Privacy
