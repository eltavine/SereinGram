#include "serein/features/stories/canvas.h"

#include "serein/features/stories/model/post.h"
#include "ui/image/image_prepare.h"

#include <QtCore/QBuffer>
#include <QtGui/QColor>
#include <QtGui/QPainter>

#include <algorithm>

namespace Serein::Stories {
namespace {

constexpr auto kBackgroundScale = 8;
constexpr auto kBackgroundBlurDivider = 11;
constexpr auto kBackgroundDimAlpha = 96;
constexpr auto kJpegQuality = 90;

[[nodiscard]] QImage BlurredBackground(const QImage &image, QSize size) {
	const auto small = QSize(
		std::max(1, size.width() / kBackgroundScale),
		std::max(1, size.height() / kBackgroundScale));
	auto result = QImage(small, QImage::Format_ARGB32_Premultiplied);
	result.fill(Qt::black);
	{
		auto p = QPainter(&result);
		p.setRenderHint(QPainter::SmoothPixmapTransform);
		p.drawImage(CoverRect(image.size(), small), image);
	}
	return Images::BlurLargeImage(
		std::move(result),
		std::max(1, small.width() / kBackgroundBlurDivider));
}

} // namespace

QImage ComposeCanvas(const QImage &image, QSize size) {
	auto result = QImage(size, QImage::Format_RGB32);
	result.fill(Qt::black);
	if (image.isNull() || size.isEmpty()) {
		return result;
	}
	{
		auto p = QPainter(&result);
		p.setRenderHint(QPainter::SmoothPixmapTransform);
		if (FillsCanvas(image.size(), size)) {
			p.drawImage(CoverRect(image.size(), size), image);
		} else {
			const auto whole = QRect(QPoint(), size);
			p.drawImage(whole, BlurredBackground(image, size));
			p.fillRect(whole, QColor(0, 0, 0, kBackgroundDimAlpha));
			p.drawImage(FitRect(image.size(), size), image);
		}
	}
	return result;
}

QByteArray EncodeJpeg(const QImage &image) {
	auto result = QByteArray();
	auto buffer = QBuffer(&result);
	if (!image.save(&buffer, "JPG", kJpegQuality)) {
		return QByteArray();
	}
	return result;
}

} // namespace Serein::Stories
