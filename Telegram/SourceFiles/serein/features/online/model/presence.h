#pragma once

namespace Serein::Online {

enum class Approximate {
	None,
	Recently,
	WithinWeek,
	WithinMonth,
	LongAgo,
	Hidden,
};

struct LastSeen {
	int onlineTill = 0;
	Approximate approximate = Approximate::None;
};

enum class PresenceKind {
	Unknown,
	Online,
	JustNow,
	Minutes,
	Hours,
	Days,
	Recently,
	WithinWeek,
	WithinMonth,
	LongAgo,
	Hidden,
};

struct Presence {
	PresenceKind kind = PresenceKind::Unknown;
	int amount = 0;

	friend inline bool operator==(const Presence &, const Presence &) = default;
};

[[nodiscard]] Presence Classify(const LastSeen &seen, int now);

} // namespace Serein::Online
