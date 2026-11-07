#include "serein/features/online/status.h"

#include "serein/features/online/model/presence.h"
#include "serein/hooks/messages/time_format.h"
#include "base/unixtime.h"
#include "data/data_lastseen_status.h"
#include "lang/lang_keys.h"

namespace Serein::Online {
namespace {

[[nodiscard]] Approximate ApproximateOf(const Data::LastseenStatus &status) {
	return status.isRecently()
		? Approximate::Recently
		: status.isWithinWeek()
		? Approximate::WithinWeek
		: status.isWithinMonth()
		? Approximate::WithinMonth
		: status.isLongAgo()
		? Approximate::LongAgo
		: status.isHidden()
		? Approximate::Hidden
		: Approximate::None;
}

[[nodiscard]] QString Amount(int value) {
	return QString::number(value);
}

} // namespace

QString CompactText(const Data::LastseenStatus &status, TimeId now) {
	const auto presence = Classify({
		.onlineTill = status.onlineTill(),
		.approximate = ApproximateOf(status),
	}, now);
	switch (presence.kind) {
	case PresenceKind::Online:
		return tr::lng_serein_online_now(tr::now);
	case PresenceKind::JustNow:
		return tr::lng_serein_online_just_now(tr::now);
	case PresenceKind::Minutes:
		return tr::lng_serein_online_minutes(
			tr::now,
			lt_amount,
			Amount(presence.amount));
	case PresenceKind::Hours:
		return tr::lng_serein_online_hours(
			tr::now,
			lt_amount,
			Amount(presence.amount));
	case PresenceKind::Days:
		return tr::lng_serein_online_days(
			tr::now,
			lt_amount,
			Amount(presence.amount));
	case PresenceKind::Recently:
		return tr::lng_serein_online_recently(tr::now);
	case PresenceKind::WithinWeek:
		return tr::lng_serein_online_week(tr::now);
	case PresenceKind::WithinMonth:
		return tr::lng_serein_online_month(tr::now);
	case PresenceKind::LongAgo:
		return tr::lng_serein_online_long_ago(tr::now);
	case PresenceKind::Hidden:
	case PresenceKind::Unknown:
		return QString();
	}
	return QString();
}

QString ExactText(
		const Data::LastseenStatus &status,
		TimeId now,
		bool seconds) {
	const auto till = status.onlineTill();
	if (till <= 0) {
		return CompactText(status, now);
	} else if (till > now) {
		return tr::lng_serein_online_now(tr::now);
	}
	const auto when = base::unixtime::parse(till);
	const auto today = base::unixtime::parse(now).date();
	const auto time = Messages::FormatTime(when.time(), seconds);
	if (when.date() == today) {
		return tr::lng_serein_online_today_at(tr::now, lt_time, time);
	} else if (when.date().addDays(1) == today) {
		return tr::lng_serein_online_yesterday_at(tr::now, lt_time, time);
	}
	return tr::lng_serein_online_date_at(
		tr::now,
		lt_date,
		langDayOfMonthShort(when.date()),
		lt_time,
		time);
}

QString LastSeenLine(
		const Data::LastseenStatus &status,
		TimeId now,
		bool seconds) {
	const auto till = status.onlineTill();
	if (till > now) {
		return tr::lng_serein_online_now(tr::now);
	}
	const auto text = ExactText(status, now, seconds);
	return text.isEmpty()
		? QString()
		: tr::lng_serein_online_last_seen(tr::now, lt_when, text);
}

} // namespace Serein::Online
