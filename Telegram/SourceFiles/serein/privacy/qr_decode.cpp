#include "serein/privacy/qr_decode.h"

#include <quirc.h>

#include <cstring>
#include <memory>

namespace Serein::Privacy {
namespace {

constexpr auto kMaxPixels = std::int64_t(64) * 1024 * 1024;

struct QuircDeleter {
	void operator()(quirc *decoder) const {
		quirc_destroy(decoder);
	}
};

} // namespace

QStringList DecodeQrCodes(const GrayImage &image) {
	auto result = QStringList();
	if (!image.pixels
		|| image.width <= 0
		|| image.height <= 0
		|| image.stride < image.width
		|| std::int64_t(image.width) * image.height > kMaxPixels) {
		return result;
	}
	const auto decoder = std::unique_ptr<quirc, QuircDeleter>(quirc_new());
	if (!decoder
		|| quirc_resize(decoder.get(), image.width, image.height) < 0) {
		return result;
	}
	auto width = 0;
	auto height = 0;
	const auto pixels = quirc_begin(decoder.get(), &width, &height);
	for (auto y = 0; y != height; ++y) {
		std::memcpy(
			pixels + std::int64_t(y) * width,
			image.pixels + std::int64_t(y) * image.stride,
			std::size_t(width));
	}
	quirc_end(decoder.get());
	for (auto i = 0, count = quirc_count(decoder.get()); i != count; ++i) {
		auto code = quirc_code();
		auto data = quirc_data();
		quirc_extract(decoder.get(), i, &code);
		auto error = quirc_decode(&code, &data);
		if (error == QUIRC_ERROR_DATA_ECC) {
			quirc_flip(&code);
			error = quirc_decode(&code, &data);
		}
		if (error == QUIRC_SUCCESS) {
			result.push_back(QString::fromUtf8(
				reinterpret_cast<const char*>(data.payload),
				data.payload_len));
		}
	}
	result.removeDuplicates();
	return result;
}

} // namespace Serein::Privacy
