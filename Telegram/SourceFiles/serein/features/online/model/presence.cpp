#include "serein/features/online/model/presence.h"

namespace Serein::Online {
namespace {

constexpr auto kMinute = 60;
constexpr auto kHour = 60 * kMinute;
constexpr auto kDay = 24 * kHour;

[[nodiscard]] PresenceKind ApproximateKind(Approximate approximate) {
	switch (approximate) {
	case Approximate::Recently: return PresenceKind::Recently;
	case Approximate::WithinWeek: return PresenceKind::WithinWeek;
	case Approximate::WithinMonth: return PresenceKind::WithinMonth;
	case Approximate::LongAgo: return PresenceKind::LongAgo;
	case Approximate::Hidden: return PresenceKind::Hidden;
	case Approximate::None: return PresenceKind::Unknown;
	}
	return PresenceKind::Unknown;
}

} // namespace

Presence Classify(const LastSeen &seen, int now) {
	if (seen.onlineTill <= 0) {
		return { .kind = ApproximateKind(seen.approximate) };
	} else if (seen.onlineTill > now) {
		return { .kind = PresenceKind::Online };
	}
	const auto ago = now - seen.onlineTill;
	if (ago < kMinute) {
		return { .kind = PresenceKind::JustNow };
	} else if (ago < kHour) {
		return { .kind = PresenceKind::Minutes, .amount = ago / kMinute };
	} else if (ago < kDay) {
		return { .kind = PresenceKind::Hours, .amount = ago / kHour };
	}
	return { .kind = PresenceKind::Days, .amount = ago / kDay };
}

} // namespace Serein::Online
