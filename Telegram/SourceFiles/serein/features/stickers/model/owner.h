#pragma once

#include <cstdint>

namespace Serein::Stickers {

[[nodiscard]] constexpr std::uint64_t SetOwnerId(std::uint64_t setId) {
	auto result = setId >> 32;
	if ((setId >> 24) & 0xFF) {
		result += 0x100000000ULL;
	}
	return result;
}

} // namespace Serein::Stickers
