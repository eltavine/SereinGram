#include "serein/network/proxy_subscription.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("ProxySubscription") {
	using namespace Serein::Network;
	const auto body = u"# my proxies\n"
		"tg://proxy?server=1.2.3.4&port=443&secret=abcdef\n"
		"<a href=\"https://t.me/proxy?server=example.org&port=8443&secret=ee00\">x</a>\n"
		"t.me/socks?server=10.0.0.1&port=1080, telegram.me/proxy?server=a.b&port=1&secret=dd;\n"
		"tg://proxy?server=1.2.3.4&port=443&secret=abcdef\n"
		"https://example.com/proxy?server=evil\n"
		"tg://resolve?domain=durov"_q;
	const auto links = ExtractProxyLinks(body);
	Require(links == QStringList{
		u"tg://proxy?server=1.2.3.4&port=443&secret=abcdef"_q,
		u"https://t.me/proxy?server=example.org&port=8443&secret=ee00"_q,
		u"t.me/socks?server=10.0.0.1&port=1080"_q,
		u"telegram.me/proxy?server=a.b&port=1&secret=dd"_q,
	}, "proxy links not extracted");
	auto many = QString();
	for (auto i = 0; i != kSubscriptionLinksLimit + 20; ++i) {
		many += u"tg://proxy?server=h%1&port=1&secret=00\n"_q.arg(i);
	}
	Require(ExtractProxyLinks(many).size() == kSubscriptionLinksLimit,
		"proxy links not capped");
	Require(ExtractProxyLinks(u"no links here"_q).isEmpty(),
		"links found in plain text");
}
