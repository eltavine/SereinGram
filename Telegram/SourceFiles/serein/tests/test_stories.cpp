#include "serein/features/stories/model/post.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("Story audience rules") {
	using namespace Serein::Stories;
	using Kind = AudienceRule::Kind;
	const auto users = std::vector<std::uint64_t>{ 7, 11 };

	Require(AudienceRules(Audience::Everyone, {})
		== std::vector<AudienceRule>{ { Kind::AllowAll } },
		"everyone without exclusions");
	Require(AudienceRules(Audience::Everyone, users)
		== std::vector<AudienceRule>{
			{ Kind::DisallowUsers, users },
			{ Kind::AllowAll },
		},
		"everyone must list exclusions before the allow rule");
	Require(AudienceRules(Audience::Contacts, users)
		== std::vector<AudienceRule>{
			{ Kind::DisallowUsers, users },
			{ Kind::AllowContacts },
		},
		"contacts with exclusions");
	Require(AudienceRules(Audience::CloseFriends, users)
		== std::vector<AudienceRule>{ { Kind::AllowCloseFriends } },
		"close friends ignore the people list");
	Require(AudienceRules(Audience::Selected, users)
		== std::vector<AudienceRule>{ { Kind::AllowUsers, users } },
		"selected people");
	Require(AudienceRules(Audience::Selected, {}).empty(),
		"nobody selected must not be postable");

	Require(TakesExclusions(Audience::Everyone)
		&& TakesExclusions(Audience::Contacts)
		&& !TakesExclusions(Audience::CloseFriends)
		&& !TakesExclusions(Audience::Selected),
		"exclusions apply to everyone and contacts only");
	Require(TakesSelection(Audience::Selected)
		&& !TakesSelection(Audience::Everyone),
		"selection applies to selected people only");
}

TEST_CASE("Story periods") {
	using namespace Serein::Stories;
	Require(PeriodChoices(false) == std::vector<int>{ kDefaultPeriod },
		"only premium accounts choose the period");
	Require(PeriodChoices(true)
		== std::vector<int>{ 21600, 43200, 86400, 172800 },
		"premium periods");
	Require(EffectivePeriod(21600, true) == 21600,
		"premium keeps a valid period");
	Require(EffectivePeriod(21600, false) == kDefaultPeriod,
		"free accounts fall back to a day");
	Require(EffectivePeriod(1000, true) == kDefaultPeriod,
		"unknown periods fall back to a day");
}

TEST_CASE("Story canvas geometry") {
	using namespace Serein::Stories;
	Require(FillsCanvas(QSize(720, 1280), kCanvasSize),
		"a 9:16 image fills the canvas");
	Require(FillsCanvas(QSize(1084, 1920), kCanvasSize),
		"small rounding differences still fill the canvas");
	Require(!FillsCanvas(QSize(1920, 1080), kCanvasSize),
		"a landscape image does not fill the canvas");
	Require(!FillsCanvas(QSize(), kCanvasSize),
		"an empty image does not fill the canvas");

	Require(FitRect(QSize(1920, 1080), kCanvasSize)
		== QRect(0, 656, 1080, 608),
		"landscape images fit to the width, centered");
	Require(FitRect(QSize(500, 2000), kCanvasSize)
		== QRect(300, 0, 480, 1920),
		"tall images fit to the height, centered");
	Require(CoverRect(QSize(1920, 1080), kCanvasSize)
		== QRect(-1166, 0, 3413, 1920),
		"landscape images cover the height");
	Require(CoverRect(QSize(1080, 1920), kCanvasSize)
		== QRect(QPoint(), kCanvasSize),
		"a canvas-sized image covers exactly");
}

TEST_CASE("Story errors") {
	using namespace Serein::Stories;
	Require(ClassifyError(u"STORIES_TOO_MUCH"_q) == PostError::TooMany,
		"active story limit");
	Require(ClassifyError(u"PREMIUM_ACCOUNT_REQUIRED"_q)
		== PostError::PremiumRequired,
		"premium limit");
	Require(ClassifyError(u"BOOSTS_REQUIRED"_q)
		== PostError::BoostsRequired,
		"channel boosts");
	Require(ClassifyError(u"STORY_SEND_FLOOD_WEEKLY_3600"_q)
		== PostError::WeeklyLimit,
		"weekly flood");
	Require(ClassifyError(u"STORY_SEND_FLOOD_MONTHLY_86400"_q)
		== PostError::MonthlyLimit,
		"monthly flood");
	Require(ClassifyError(u"MEDIA_INVALID"_q) == PostError::Other,
		"other errors");
}
