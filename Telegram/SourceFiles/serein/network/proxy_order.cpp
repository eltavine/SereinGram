#include "serein/network/proxy_order.h"

#include <algorithm>
#include <numeric>

namespace Serein::Network {
namespace {

[[nodiscard]] int Rank(const ProxyLatency &entry) {
	return entry.ping ? 0 : !entry.tested ? 1 : 2;
}

} // namespace

std::vector<int> LatencyOrder(const std::vector<ProxyLatency> &list) {
	auto result = std::vector<int>(list.size());
	std::iota(result.begin(), result.end(), 0);
	std::stable_sort(result.begin(), result.end(), [&](int a, int b) {
		const auto &first = list[a];
		const auto &second = list[b];
		const auto rankFirst = Rank(first);
		const auto rankSecond = Rank(second);
		if (rankFirst != rankSecond) {
			return rankFirst < rankSecond;
		}
		return (rankFirst == 0) && (*first.ping < *second.ping);
	});
	return result;
}

std::vector<int> UnavailableProxies(const std::vector<ProxyLatency> &list) {
	auto result = std::vector<int>();
	for (auto i = 0; i != int(list.size()); ++i) {
		const auto &entry = list[i];
		if (entry.tested && !entry.ping && !entry.selected) {
			result.push_back(i);
		}
	}
	return result;
}

} // namespace Serein::Network
