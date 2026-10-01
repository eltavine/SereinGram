#pragma once

#include <optional>
#include <vector>

namespace Serein::Network {

struct ProxyLatency {
	std::optional<int> ping;
	bool tested = true;
	bool selected = false;
};

[[nodiscard]] std::vector<int> LatencyOrder(
	const std::vector<ProxyLatency> &list);
[[nodiscard]] std::vector<int> UnavailableProxies(
	const std::vector<ProxyLatency> &list);

} // namespace Serein::Network
