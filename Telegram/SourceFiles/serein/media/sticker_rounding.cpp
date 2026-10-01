#include "serein/hooks/media/stickers.h"

#include "serein/hooks/gen/media.h"

namespace Serein::Hooks::Media {

Images::Options StickerRoundOptions(bool emoji) {
	return (!emoji && RoundedStickers())
		? Images::RoundOptions(ImageRoundRadius::Large)
		: Images::Options();
}

QImage RoundStickerFrame(const QImage &frame, bool emoji) {
	return (emoji || frame.isNull() || !RoundedStickers())
		? frame
		: Images::Round(QImage(frame), ImageRoundRadius::Large);
}

} // namespace Serein::Hooks::Media
