#pragma once

#include "ui/image/image_prepare.h"

namespace Serein::Hooks::Media {

[[nodiscard]] Images::Options StickerRoundOptions(bool emoji);
[[nodiscard]] QImage RoundStickerFrame(const QImage &frame, bool emoji);

} // namespace Serein::Hooks::Media
