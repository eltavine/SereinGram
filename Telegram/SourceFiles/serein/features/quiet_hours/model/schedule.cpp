#include "serein/features/quiet_hours/model/schedule.h"

#include "serein/schema/gen/settings/interface.h"

#include <algorithm>
#include <set>

namespace Serein::Notifications {
namespace {

[[nodiscard]] bool Selected(const QuietHours &config, int weekday) {
	return config.weekdays.empty()
		|| std::ranges::find(config.weekdays, weekday) != config.weekdays.end();
}

[[nodiscard]] int Previous(int weekday) {
	return (weekday == 1) ? 7 : (weekday - 1);
}

} // namespace

bool ValidQuietHours(const QuietHours &value) {
	auto seen = std::set<int>();
	return std::ranges::all_of(value.weekdays, [&](int weekday) {
		return weekday >= 1 && weekday <= 7 && seen.insert(weekday).second;
	});
}

std::optional<QuietHours> ReadQuietHours(const QByteArray &raw) {
	return raw.isEmpty()
		? std::make_optional(QuietHours())
		: ParseQuietHours(raw);
}

bool Active(const QuietHours &config, int isoWeekday, int minuteOfDay) {
	if (!config.enabled) {
		return false;
	}
	const auto start = config.startMinute;
	const auto end = config.endMinute;
	const auto minute = minuteOfDay;
	if (start == end) {
		return Selected(config, isoWeekday);
	} else if (start < end) {
		return Selected(config, isoWeekday) && minute >= start && minute < end;
	}
	return (minute >= start && Selected(config, isoWeekday))
		|| (minute < end && Selected(config, Previous(isoWeekday)));
}

bool Silences(
		const QuietHours &config,
		const NotificationFacts &facts,
		int isoWeekday,
		int minuteOfDay) {
	if (!Active(config, isoWeekday, minuteOfDay)) {
		return false;
	} else if (!facts.message) {
		return true;
	}
	const auto allowed = (config.allowContacts && facts.fromContact)
		|| (config.allowPinned && facts.pinnedChat)
		|| (config.allowMentions && facts.mentionsMe)
		|| (config.allowKeywords && facts.keywordMatch);
	return !allowed;
}

} // namespace Serein::Notifications

namespace Serein::Interface {

bool ValidQuietHoursBytes(const QByteArray &value) {
	return Notifications::ReadQuietHours(value).has_value();
}

} // namespace Serein::Interface
