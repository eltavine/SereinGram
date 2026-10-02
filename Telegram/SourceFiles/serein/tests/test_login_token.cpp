#include "serein/privacy/login_token.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <iostream>

TEST_CASE("Login token") {
	using Serein::Privacy::ParseLoginToken;
	const auto token = ParseLoginToken(u"tg://login?token=AQIDBP-_"_q);
	Require(token
		&& *token == QByteArray::fromHex("01020304ffbf"),
		"base64url token not decoded");
	Require(ParseLoginToken(u"  tg://login?token=AQID  "_q)
			== QByteArray::fromHex("010203"),
		"token with spaces not decoded");
	Require(!ParseLoginToken(u"https://login?token=AQID"_q), "other scheme accepted");
	Require(!ParseLoginToken(u"tg://resolve?token=AQID"_q), "other host accepted");
	Require(!ParseLoginToken(u"tg://login?code=AQID"_q), "missing token accepted");
	Require(!ParseLoginToken(u"tg://login?token="_q), "empty token accepted");
	Require(!ParseLoginToken(u"tg://login?token=A*B"_q), "invalid base64 accepted");
	Require(!ParseLoginToken(
			u"tg://login?token="_q + QString(2000, u'A')),
		"oversized token accepted");
	std::cout << "PASS: Serein login token" << std::endl;
}
