#include "serein/features/stickers/model/owner.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("Stickers") {
	using Serein::Stickers::SetOwnerId;
	constexpr auto owner = std::uint64_t(123456789);
	Require(SetOwnerId((owner << 32) | 0x00ABCDEFULL) == owner,
		"sticker set owner with an empty marker byte");
	Require(SetOwnerId((owner << 32) | 0x01ABCDEFULL)
			== owner + 0x100000000ULL,
		"sticker set owner with a marker byte");
	Require(SetOwnerId(0x00FFFFFFULL) == 0,
		"sticker set without an owner");
}
