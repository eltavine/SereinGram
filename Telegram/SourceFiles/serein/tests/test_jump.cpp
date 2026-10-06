#include "serein/features/jump/model/target.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

namespace {

using Serein::Jump::ParseTarget;
using Serein::Jump::Target;

[[nodiscard]] bool Is(
		const std::optional<Target> &target,
		qint64 messageId,
		qint64 channelId = 0,
		const QString &username = QString()) {
	return target
		&& target->messageId == messageId
		&& target->channelId == channelId
		&& target->username == username;
}

} // namespace

TEST_CASE("JumpParsesMessageIds") {
	Require(Is(ParseTarget(u"123"_q), 123), "plain id");
	Require(Is(ParseTarget(u"  #45 "_q), 45), "hash and spaces");
	Require(!ParseTarget(u"0"_q), "zero");
	Require(!ParseTarget(u"-5"_q), "negative");
	Require(!ParseTarget(u"99999999999"_q), "beyond message ids");
	Require(!ParseTarget(QString()), "empty");
}

TEST_CASE("JumpParsesMessageLinks") {
	Require(
		Is(ParseTarget(u"https://t.me/c/1234567890/77"_q), 77, 1234567890),
		"private channel link");
	Require(
		Is(ParseTarget(u"t.me/c/1234567890/12/77"_q), 77, 1234567890),
		"topic in a private link");
	Require(
		Is(ParseTarget(u"https://t.me/durov/123"_q), 123, 0, u"durov"_q),
		"public link");
	Require(
		Is(ParseTarget(u"t.me/s/durov/123"_q), 123, 0, u"durov"_q),
		"web preview link");
	Require(
		Is(ParseTarget(u"https://t.me/durov/5/123?comment=2"_q),
			123,
			0,
			u"durov"_q),
		"topic and query");
	Require(
		Is(ParseTarget(u"telegram.me/durov/9"_q), 9, 0, u"durov"_q),
		"old domain");
	Require(
		Is(ParseTarget(u"durov.t.me/15"_q), 15, 0, u"durov"_q),
		"username subdomain");
	Require(
		Is(ParseTarget(u"tg://privatepost?channel=1234567890&post=9"_q),
			9,
			1234567890),
		"private deep link");
	Require(
		Is(ParseTarget(u"tg://resolve?domain=durov&post=10"_q),
			10,
			0,
			u"durov"_q),
		"public deep link");
}

TEST_CASE("JumpRejectsOtherLinks") {
	Require(!ParseTarget(u"https://example.com/durov/1"_q), "other host");
	Require(!ParseTarget(u"t.me/joinchat/abc"_q), "invite link");
	Require(!ParseTarget(u"t.me/durov"_q), "no message id");
	Require(!ParseTarget(u"t.me/c/77"_q), "channel without message");
	Require(!ParseTarget(u"t.me/ab/1"_q), "username too short");
	Require(!ParseTarget(u"tg://resolve?domain=durov"_q), "deep link without post");
	Require(!ParseTarget(u"ftp://t.me/durov/1"_q), "other scheme");
}
