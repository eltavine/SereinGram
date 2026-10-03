#include "serein/features/cleanup/model/candidates.h"

#include <doctest/doctest.h>

namespace Serein::Cleanup {
namespace {

constexpr auto kDay = 86400;
constexpr auto kNow = TimeId(1'800'000'000);

} // namespace

TEST_CASE("ChatCleanup") {
	SUBCASE("deleted accounts are offered regardless of activity") {
		CHECK(Classify({ .user = true, .deleted = true }, kNow)
			== Category::DeletedAccount);
		CHECK(Classify({
			.user = true,
			.deleted = true,
			.lastActivity = kNow,
		}, kNow) == Category::DeletedAccount);
	}
	SUBCASE("bots and chats are offered only after the inactive period") {
		const auto old = kNow - kInactiveDays * kDay;
		const auto recent = old + 1;
		const auto bot = [](TimeId lastActivity) {
			return ChatFacts{
				.user = true,
				.bot = true,
				.lastActivity = lastActivity,
			};
		};
		CHECK(Classify(bot(old), kNow) == Category::UnusedBot);
		CHECK(!Classify(bot(recent), kNow));
		CHECK(Classify({ .lastActivity = old }, kNow) == Category::InactiveChat);
		CHECK(!Classify({ .lastActivity = recent }, kNow));
		CHECK(Classify({ .lastActivity = kNow - 8 * kDay }, kNow, 7)
			== Category::InactiveChat);
	}
	SUBCASE("people, unknown activity and kept chats are never offered") {
		const auto old = kNow - 400 * kDay;
		CHECK(!Classify({ .user = true, .lastActivity = old }, kNow));
		CHECK(!Classify({ .user = true, .bot = true }, kNow));
		CHECK(!Classify({}, kNow));
		CHECK(!Classify({ .kept = true, .lastActivity = old }, kNow));
		CHECK(!Classify({ .user = true, .deleted = true, .kept = true }, kNow));
	}
}

} // namespace Serein::Cleanup
