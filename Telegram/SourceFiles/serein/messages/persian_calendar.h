#pragma once

#include <QtCore/QDate>
#include <QtCore/QLocale>
#include <QtCore/QString>

#include <optional>

namespace Serein::Messages {

struct PersianDate {
	int year = 0;
	int month = 0;
	int day = 0;
};

[[nodiscard]] std::optional<PersianDate> ToPersian(const QDate &date);
[[nodiscard]] QString PersianMonthName(
	const QLocale &locale,
	const PersianDate &date);
[[nodiscard]] QString PersianShortDate(
	const QLocale &locale,
	const QDate &date);

} // namespace Serein::Messages
