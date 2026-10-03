#include "serein/features/quiet_hours/model/schedule.h"
#include "serein/schema/gen/settings/interface.h"

#include <doctest/doctest.h>

namespace Serein::Notifications {
namespace {

constexpr auto kHour = 60;
constexpr auto kMonday = 1;
constexpr auto kFriday = 5;
constexpr auto kSaturday = 6;
constexpr auto kSunday = 7;

[[nodiscard]] QuietHours Period(
		int start,
		int end,
		std::vector<int> weekdays = {}) {
	return QuietHours{
		.enabled = true,
		.startMinute = start,
		.endMinute = end,
		.weekdays = std::move(weekdays),
	};
}

} // namespace

TEST_CASE("QuietHours") {
	SUBCASE("disabled by default") {
		CHECK(!Active(QuietHours(), kMonday, 0));
		CHECK(ReadQuietHours(QByteArray()) == QuietHours());
		CHECK(!ReadQuietHours("{}"));
	}
	SUBCASE("a period within one day") {
		const auto config = Period(9 * kHour, 17 * kHour);
		CHECK(!Active(config, kMonday, 9 * kHour - 1));
		CHECK(Active(config, kMonday, 9 * kHour));
		CHECK(Active(config, kMonday, 17 * kHour - 1));
		CHECK(!Active(config, kMonday, 17 * kHour));
	}
	SUBCASE("a period past midnight belongs to its start day") {
		const auto config = Period(22 * kHour, 7 * kHour, { kFriday });
		CHECK(Active(config, kFriday, 23 * kHour));
		CHECK(Active(config, kSaturday, 6 * kHour));
		CHECK(!Active(config, kSaturday, 7 * kHour));
		CHECK(!Active(config, kFriday, 6 * kHour));
		CHECK(!Active(config, kSaturday, 23 * kHour));
		CHECK(Active(Period(22 * kHour, 7 * kHour, { kSunday }), kMonday, 0));
	}
	SUBCASE("the same start and end cover the whole day") {
		CHECK(Active(Period(0, 0, { kMonday }), kMonday, 0));
		CHECK(Active(Period(10 * kHour, 10 * kHour), kSunday, kMinutesPerDay - 1));
		CHECK(!Active(Period(10 * kHour, 10 * kHour, { kMonday }), kFriday, 0));
	}
	SUBCASE("exceptions apply to messages only") {
		auto config = Period(0, 0);
		const auto contact = NotificationFacts{
			.message = true,
			.fromContact = true,
		};
		CHECK(Silences(config, contact, kMonday, 0));
		config.allowContacts = true;
		CHECK(!Silences(config, contact, kMonday, 0));
		CHECK(Silences(config, { .fromContact = true }, kMonday, 0));
		CHECK(Silences(config, { .message = true, .mentionsMe = true }, kMonday, 0));
		config.allowMentions = true;
		CHECK(!Silences(config, { .message = true, .mentionsMe = true }, kMonday, 0));
		config.allowKeywords = true;
		CHECK(!Silences(config, { .message = true, .keywordMatch = true }, kMonday, 0));
		config.allowPinned = true;
		CHECK(!Silences(config, { .message = true, .pinnedChat = true }, kMonday, 0));
		CHECK(!Silences(Period(0, kHour), { .message = true }, kMonday, kHour));
	}
	SUBCASE("documents") {
		auto config = Period(22 * kHour, 7 * kHour, { 1, 2, 3 });
		config.allowPinned = true;
		CHECK(ReadQuietHours(SerializeQuietHours(config)) == config);
		CHECK(Interface::ValidQuietHoursBytes(SerializeQuietHours(config)));
		CHECK(!ReadQuietHours(SerializeQuietHours(Period(0, kMinutesPerDay))));
		CHECK(!ReadQuietHours(SerializeQuietHours(Period(-1, 0))));
		CHECK(!ReadQuietHours(SerializeQuietHours(Period(0, kHour, { 1, 1 }))));
		CHECK(!ReadQuietHours(SerializeQuietHours(Period(0, kHour, { 8 }))));
		CHECK(!Interface::ValidQuietHoursBytes(R"({"version":2})"));
	}
}

} // namespace Serein::Notifications
