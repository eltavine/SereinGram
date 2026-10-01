#include "serein/messages/persian_calendar.h"
#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

[[nodiscard]] bool Is(
		const QDate &date,
		int year,
		int month,
		int day) {
	const auto persian = Serein::Messages::ToPersian(date);
	return persian
		&& persian->year == year
		&& persian->month == month
		&& persian->day == day;
}

} // namespace

void TestPersianCalendar() {
	using namespace Serein::Messages;
	Require(Is(QDate(2024, 3, 20), 1403, 1, 1), "Nowruz 1403");
	Require(Is(QDate(2024, 3, 19), 1402, 12, 29), "last day of 1402");
	Require(Is(QDate(2025, 3, 20), 1403, 12, 30), "leap Esfand 30");
	Require(Is(QDate(2026, 10, 1), 1405, 7, 9), "Mehr 9, 1405");
	Require(!ToPersian(QDate()), "invalid date converted");
	const auto english = QLocale(QLocale::English);
	Require(PersianMonthName(english, { 1403, 1, 1 }) == u"Farvardin"_q,
		"English month name");
	Require(PersianMonthName(QLocale(QLocale::Persian), { 1403, 12, 1 })
			== QString::fromUtf8("\xd8\xa7\xd8\xb3\xd9\x81\xd9\x86\xd8\xaf"),
		"Persian month name");
	Require(!PersianShortDate(english, QDate(2024, 3, 20)).isEmpty(),
		"short date");
}
