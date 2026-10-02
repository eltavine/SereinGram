#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QSize>
#include <QtGui/QImage>

namespace Serein::Stories {

[[nodiscard]] QImage ComposeCanvas(const QImage &image, QSize size);
[[nodiscard]] QByteArray EncodeJpeg(const QImage &image);

} // namespace Serein::Stories
