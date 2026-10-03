#include "serein/features/keyword_alerts/model/keywords.h"
#include "serein/schema/gen/settings/filters.h"
#include "base/basic_types.h"

#include <doctest/doctest.h>

namespace Serein::Notifications {
namespace {

[[nodiscard]] KeywordAlerts Enabled(const QString &lines) {
	return KeywordAlerts{
		.enabled = true,
		.rules = ParseKeywordLines(lines),
	};
}

} // namespace

TEST_CASE("KeywordAlerts") {
	SUBCASE("lines") {
		const auto rules = ParseKeywordLines(
			u"  release \n\n/v\\d+/i\n/Exact/\nrelease\n/unclosed\n//i"_q);
		REQUIRE(rules.size() == 5);
		CHECK(rules[0] == KeywordRule{ .pattern = u"release"_q });
		CHECK(rules[1] == KeywordRule{
			.pattern = u"v\\d+"_q,
			.regex = true,
		});
		CHECK(rules[2] == KeywordRule{
			.pattern = u"Exact"_q,
			.regex = true,
			.caseSensitive = true,
		});
		CHECK(rules[3] == KeywordRule{ .pattern = u"/unclosed"_q });
		CHECK(rules[4] == KeywordRule{ .pattern = u"//i"_q });
		CHECK(FormatKeywordLines(rules)
			== u"release\n/v\\d+/i\n/Exact/\n/unclosed\n//i"_q);
		CHECK(ParseKeywordLines(FormatKeywordLines(rules)) == rules);
	}
	SUBCASE("matching") {
		auto config = Enabled(
			u"Release\n/build \\d+/\n/\u0443\u0440\u043E\u043A/i"_q);
		CHECK(Matches(config, u"a new release today"_q, false));
		CHECK(Matches(config, u"build 42 is out"_q, false));
		CHECK(!Matches(config, u"Build 42 is out"_q, false));
		CHECK(Matches(config, u"\u0423\u0420\u041E\u041A"_q, false));
		CHECK(!Matches(config, u"nothing here"_q, false));
		CHECK(!Matches(config, QString(), false));
		CHECK(!Matches(config, u"release"_q, true));
		config.includeChannels = true;
		CHECK(Matches(config, u"release"_q, true));
		config.enabled = false;
		CHECK(!Matches(config, u"release"_q, false));
	}
	SUBCASE("pathological patterns fail instead of hanging") {
		const auto config = Enabled(u"/(a|aa)+$/"_q);
		CHECK(!Matches(config, QString(64, u'a') + u'b', false));
	}
	SUBCASE("documents") {
		auto config = Enabled(u"a\n/b+/"_q);
		config.includeChannels = true;
		CHECK(ReadKeywordAlerts(SerializeKeywordAlerts(config)) == config);
		CHECK(ReadKeywordAlerts(QByteArray()) == KeywordAlerts());
		CHECK(Filters::ValidKeywordAlertsBytes(SerializeKeywordAlerts(config)));
		CHECK(!Filters::ValidKeywordAlertsBytes("not json"));

		auto invalid = config;
		invalid.rules.push_back({ .pattern = u"(open"_q, .regex = true });
		CHECK(!ValidKeywordRule(invalid.rules.back()));
		CHECK(!ReadKeywordAlerts(SerializeKeywordAlerts(invalid)));

		auto duplicate = config;
		duplicate.rules.push_back(duplicate.rules.front());
		CHECK(!ReadKeywordAlerts(SerializeKeywordAlerts(duplicate)));

		auto many = KeywordAlerts{ .enabled = true };
		for (auto i = 0; i != kMaxKeywordRules + 1; ++i) {
			many.rules.push_back({ .pattern = QString::number(i) });
		}
		CHECK(!ReadKeywordAlerts(SerializeKeywordAlerts(many)));
		many.rules.pop_back();
		CHECK(ReadKeywordAlerts(SerializeKeywordAlerts(many)) == many);
	}
}

} // namespace Serein::Notifications
