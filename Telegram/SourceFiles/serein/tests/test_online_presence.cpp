#include "serein/features/online/model/presence.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

#include <iostream>

TEST_CASE("OnlinePresence") {
	using namespace Serein::Online;
	constexpr auto now = 1'800'000'000;
	const auto at = [](int till) {
		return Classify({ .onlineTill = till }, now);
	};
	Require(at(now + 30) == Presence{ PresenceKind::Online },
		"a future expiry means online");
	Require(at(now - 59) == Presence{ PresenceKind::JustNow },
		"under a minute is just now");
	Require(at(now - 5 * 60) == Presence{ PresenceKind::Minutes, 5 },
		"minutes are counted down");
	Require(at(now - 3 * 3600 - 10) == Presence{ PresenceKind::Hours, 3 },
		"hours are counted down");
	Require(at(now - 2 * 86400) == Presence{ PresenceKind::Days, 2 },
		"days are counted down");
	const auto approximate = [](Approximate value) {
		return Classify({ .approximate = value }, now).kind;
	};
	Require(approximate(Approximate::Recently) == PresenceKind::Recently
		&& approximate(Approximate::WithinWeek) == PresenceKind::WithinWeek
		&& approximate(Approximate::WithinMonth) == PresenceKind::WithinMonth
		&& approximate(Approximate::LongAgo) == PresenceKind::LongAgo
		&& approximate(Approximate::Hidden) == PresenceKind::Hidden
		&& approximate(Approximate::None) == PresenceKind::Unknown,
		"approximate statuses keep their bucket");
	std::cout << "PASS: Serein online presence" << std::endl;
}
