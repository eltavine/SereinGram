#include "serein/compose/mention_query.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

namespace {

[[nodiscard]] Serein::Compose::MentionQuery Parse(const char *input) {
	return Serein::Compose::ParseMentionQuery(QString::fromUtf8(input));
}

} // namespace

TEST_CASE("MentionQuery") {
	for (const auto input : {
			"@durov",
			"durov",
			" t.me/durov ",
			"https://t.me/durov",
			"http://telegram.me/durov",
			"HTTPS://T.ME/durov" }) {
		const auto query = Parse(input);
		Require(!query.userId && query.username == QString::fromLatin1("durov"),
			"username mention not parsed");
	}
	Require(Parse("12345").userId == 12345, "user id not parsed");
	Require(Parse("@777").userId == 777, "prefixed user id not parsed");
	Require(Parse("tg://user?id=42").userId == 42, "user link not parsed");
	Require(Parse("t.me/1234567").userId == 1234567, "t.me user id not parsed");
	for (const auto bad : {
			"",
			"@",
			"0",
			"01",
			"abc",
			"a b",
			"_durov",
			"9durov",
			"12345678901234567",
			"tg://user?id=0",
			"https://example.com/durov" }) {
		const auto query = Parse(bad);
		Require(!query.userId && query.username.isEmpty(),
			"malformed mention accepted");
	}
}
