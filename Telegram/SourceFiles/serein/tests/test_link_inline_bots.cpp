#include "serein/compose/link_inline_bots.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <iostream>

TEST_CASE("Link inline bots") {
	using namespace Serein::Compose;
	const auto parsed = ParseLinkInlineBotLines(
		u"@vid => youtube\\.com|youtu\\.be\n"
		"\n"
		"fxtwitter_bot => ^https://(x|twitter)\\.com/"_q);
	Require(parsed && parsed->rules.size() == 2, "rules not parsed");
	Require(parsed->rules[0].bot == u"vid"_q, "leading @ kept in the bot");
	Require(FormatLinkInlineBotLines(*parsed)
		== u"@vid => youtube\\.com|youtu\\.be\n"
			"@fxtwitter_bot => ^https://(x|twitter)\\.com/"_q,
		"rules not formatted back");
	Require(!ParseLinkInlineBotLines(u"@vid youtube"_q), "line without arrow accepted");
	Require(!ParseLinkInlineBotLines(u"@1vid => youtube"_q), "invalid bot accepted");
	Require(!ParseLinkInlineBotLines(u"@vid => ("_q), "invalid pattern accepted");
	Require(!ParseLinkInlineBotLines(u"@vid => "_q), "empty pattern accepted");

	const auto matcher = LinkInlineBotMatcher(*parsed);
	Require(matcher.match(u"https://www.YouTube.com/watch?v=1"_q) == u"vid"_q,
		"matching link not found");
	Require(matcher.match(u"  https://x.com/a/status/1 "_q) == u"fxtwitter_bot"_q,
		"surrounding spaces not ignored");
	Require(matcher.match(u"look https://youtu.be/1"_q).isEmpty(),
		"message with words matched");
	Require(matcher.match(u"@vid https://youtu.be/1"_q).isEmpty(),
		"explicit inline query matched");
	Require(matcher.match(u"https://example.com/"_q).isEmpty(),
		"unrelated link matched");
	Require(matcher.match(QString(kMaxInlineBotQuery, u'a') + u"youtube.com"_q)
			.isEmpty(),
		"overlong draft matched");

	Require(ReadLinkInlineBots(WriteLinkInlineBots(*parsed)) == parsed,
		"rules not stored losslessly");
	Require(ReadLinkInlineBots(QByteArray())
			&& ReadLinkInlineBots(QByteArray())->rules.empty(),
		"empty setting rejected");
	Require(WriteLinkInlineBots(LinkInlineBots()).isEmpty(),
		"empty rules stored as a document");
	std::cout << "PASS: Serein link inline bots" << std::endl;
}
