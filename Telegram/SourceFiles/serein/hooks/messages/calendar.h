#pragma once

#include <QtCore/QString>

#include <optional>

class QDate;

namespace Serein::Messages {

[[nodiscard]] std::optional<QString> PersianDayOfMonth(const QDate &date);
[[nodiscard]] std::optional<QString> PersianDayOfMonthShort(
	const QDate &date);
[[nodiscard]] std::optional<QString> PersianMonth(const QDate &date);

} // namespace Serein::Messages
