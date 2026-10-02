#include "serein/privacy/qr_decode.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <qrcodegen.hpp>

#include <cstdint>
#include <iostream>
#include <vector>

namespace {

constexpr auto kScale = 4;
constexpr auto kBorder = 4;

[[nodiscard]] std::vector<std::uint8_t> Render(
		const qrcodegen::QrCode &code,
		int *side) {
	*side = (code.getSize() + kBorder * 2) * kScale;
	auto result = std::vector<std::uint8_t>(
		std::size_t(*side) * std::size_t(*side),
		std::uint8_t(0xFF));
	for (auto y = 0; y != code.getSize(); ++y) {
		for (auto x = 0; x != code.getSize(); ++x) {
			if (!code.getModule(x, y)) {
				continue;
			}
			for (auto dy = 0; dy != kScale; ++dy) {
				const auto row = (y + kBorder) * kScale + dy;
				for (auto dx = 0; dx != kScale; ++dx) {
					const auto column = (x + kBorder) * kScale + dx;
					result[std::size_t(row) * std::size_t(*side)
						+ std::size_t(column)] = 0;
				}
			}
		}
	}
	return result;
}

} // namespace

TEST_CASE("QR decoding") {
	using namespace Serein::Privacy;
	const auto text = "tg://login?token=AQIDBP-_";
	const auto code = qrcodegen::QrCode::encodeText(
		text,
		qrcodegen::QrCode::Ecc::MEDIUM);
	auto side = 0;
	const auto pixels = Render(code, &side);
	Require(DecodeQrCodes({ pixels.data(), side, side, side })
			== QStringList{ QString::fromLatin1(text) },
		"rendered QR code not decoded");
	const auto blank = std::vector<std::uint8_t>(
		std::size_t(64) * std::size_t(64),
		std::uint8_t(0xFF));
	Require(DecodeQrCodes({ blank.data(), 64, 64, 64 }).isEmpty(),
		"blank image decoded");
	Require(DecodeQrCodes({}).isEmpty(), "empty image decoded");
	Require(DecodeQrCodes({ blank.data(), 64, 64, 32 }).isEmpty(),
		"stride shorter than the width accepted");
	std::cout << "PASS: Serein QR decoding" << std::endl;
}
