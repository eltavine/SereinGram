#include "serein/messages/persian_calendar.h"

#include <QtCore/QCalendar>

namespace Serein::Messages {

std::optional<PersianDate> ToPersian(const QDate &date) {
#if QT_CONFIG(jalalicalendar)
	if (date.isValid()) {
		const auto parts = QCalendar(
			QCalendar::System::Jalali).partsFromDate(date);
		if (parts.isValid()) {
			return PersianDate{ parts.year, parts.month, parts.day };
		}
	}
#endif // QT_CONFIG(jalalicalendar)
	return std::nullopt;
}

QString PersianMonthName(const QLocale &locale, const PersianDate &date) {
#if QT_CONFIG(jalalicalendar)
	return QCalendar(QCalendar::System::Jalali).monthName(
		locale,
		date.month,
		date.year);
#else // QT_CONFIG(jalalicalendar)
	return QString::number(date.month);
#endif // QT_CONFIG(jalalicalendar)
}

QString PersianShortDate(const QLocale &locale, const QDate &date) {
#if QT_CONFIG(jalalicalendar)
	return locale.toString(
		date,
		QLocale::ShortFormat,
		QCalendar(QCalendar::System::Jalali));
#else // QT_CONFIG(jalalicalendar)
	return locale.toString(date, QLocale::ShortFormat);
#endif // QT_CONFIG(jalalicalendar)
}

} // namespace Serein::Messages
