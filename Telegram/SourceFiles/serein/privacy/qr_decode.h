#pragma once

#include <QtCore/QStringList>

#include <cstdint>

namespace Serein::Privacy {

struct GrayImage {
	const std::uint8_t *pixels = nullptr;
	int width = 0;
	int height = 0;
	int stride = 0;
};

[[nodiscard]] QStringList DecodeQrCodes(const GrayImage &image);

} // namespace Serein::Privacy
