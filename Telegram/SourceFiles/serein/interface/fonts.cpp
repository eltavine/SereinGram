#include "serein/interface/fonts.h"

#include "serein/interface/options.h"
#include "ui/style/style_core_font.h"

#include <QtGui/QFontDatabase>

namespace Serein::Interface {

void StartFonts() {
	for (const auto file : {
			":/serein/fonts/unifont.otf",
			":/serein/fonts/unifont_upper.otf",
			":/serein/fonts/NotoSansBamum-Regular.ttf",
			":/serein/fonts/NotoSansLinearA-Regular.ttf",
			":/serein/fonts/NotoSansTaiTham-Regular.ttf" }) {
		if (QFontDatabase::addApplicationFont(QString::fromLatin1(file)) < 0) {
			LOG(("Font Error: Could not load Unicode fallback font."));
		}
	}
	const auto family = ForDevice().Get(kMonospaceFont);
	style::SetCustomMonospaceFont(
		QFontDatabase::hasFamily(family) ? family : QString(),
		ForDevice().Get(kMonospaceLigatures));
}

} // namespace Serein::Interface
