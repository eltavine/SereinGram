#include "serein/network/proxy_order.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("ProxyOrder") {
	using namespace Serein::Network;
	const auto list = std::vector<ProxyLatency>{
		{ .ping = std::nullopt },
		{ .ping = 300 },
		{ .ping = std::nullopt, .tested = false },
		{ .ping = 80 },
		{ .ping = std::nullopt, .selected = true },
		{ .ping = 300 },
	};
	Require(LatencyOrder(list) == std::vector<int>{ 3, 1, 5, 2, 0, 4 },
		"proxies not ordered by latency");
	Require(UnavailableProxies(list) == std::vector<int>{ 0 },
		"wrong unavailable proxies");
	Require(LatencyOrder({}).empty() && UnavailableProxies({}).empty(),
		"empty proxy list");
}
