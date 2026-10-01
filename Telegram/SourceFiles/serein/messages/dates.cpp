#include "serein/hooks/messages/calendar.h"

#include "serein/messages/options.h"
#include "serein/messages/persian_calendar.h"
#include "lang/lang_instance.h"
#include "lang/lang_keys.h"

namespace Serein::Messages {
namespace {

[[nodiscard]] QLocale InterfaceLocale() {
	auto parts = Lang::GetInstance().id().toLower().split(u'-');
	if (parts.size() > 1 && parts[1].size() == 4) {
		parts[1][0] = parts[1][0].toUpper();
		return QLocale(parts[0] + u'-' + parts[1]);
	}
	return QLocale(parts[0]);
}

[[nodiscard]] std::optional<PersianDate> Enabled(const QDate &date) {
	return ForDevice().Get(kPersianCalendar)
		? ToPersian(date)
		: std::nullopt;
}

[[nodiscard]] bool ShowYear(const PersianDate &date) {
	const auto now = ToPersian(QDate::currentDate());
	return !now || (now->year != date.year);
}

} // namespace

std::optional<QString> PersianDayOfMonth(const QDate &date) {
	const auto persian = Enabled(date);
	if (!persian) {
		return std::nullopt;
	}
	const auto month = PersianMonthName(InterfaceLocale(), *persian);
	const auto day = QString::number(persian->day);
	return ShowYear(*persian)
		? tr::lng_month_day_year(
			tr::now,
			lt_month,
			month,
			lt_day,
			day,
			lt_year,
			QString::number(persian->year))
		: tr::lng_month_day(tr::now, lt_month, month, lt_day, day);
}

std::optional<QString> PersianDayOfMonthShort(const QDate &date) {
	const auto persian = Enabled(date);
	if (!persian) {
		return std::nullopt;
	}
	return ShowYear(*persian)
		? std::make_optional(PersianShortDate(QLocale(), date))
		: PersianDayOfMonth(date);
}

std::optional<QString> PersianMonth(const QDate &date) {
	const auto persian = Enabled(date);
	if (!persian) {
		return std::nullopt;
	}
	const auto month = PersianMonthName(InterfaceLocale(), *persian);
	return ShowYear(*persian)
		? tr::lng_month_year(
			tr::now,
			lt_month,
			month,
			lt_year,
			QString::number(persian->year))
		: month;
}

} // namespace Serein::Messages
