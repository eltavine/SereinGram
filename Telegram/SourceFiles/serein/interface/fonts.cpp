#include "serein/interface/fonts.h"

#include "serein/interface/options.h"
#include "ui/style/style_core_font.h"

#include <QtGui/QFontDatabase>

#include <array>

namespace Serein::Interface {

void StartFonts() {
	const auto fonts = std::array{
		std::pair{ ":/serein/fonts/unifont.otf", QChar::Script_Unknown },
		std::pair{ ":/serein/fonts/unifont_upper.otf", QChar::Script_Unknown },
		std::pair{ ":/serein/fonts/NotoSansBamum-Regular.ttf", QChar::Script_Bamum },
		std::pair{ ":/serein/fonts/NotoSansLinearA-Regular.ttf", QChar::Script_LinearA },
		std::pair{ ":/serein/fonts/NotoSansTaiTham-Regular.ttf", QChar::Script_TaiTham },
	};
	for (const auto &[file, script] : fonts) {
		const auto id = QFontDatabase::addApplicationFont(QString::fromLatin1(file));
		if (id < 0) {
			LOG(("Font Error: Could not load Unicode fallback font %1."
				).arg(QString::fromLatin1(file)));
			continue;
		}
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
		if (script != QChar::Script_Unknown) {
			for (const auto &family : QFontDatabase::applicationFontFamilies(id)) {
				QFontDatabase::addApplicationFallbackFontFamily(script, family);
			}
		}
#endif
	}
	const auto family = ForDevice().Get(kMonospaceFont);
	style::SetCustomMonospaceFont(
		QFontDatabase::hasFamily(family) ? family : QString(),
		ForDevice().Get(kMonospaceLigatures));
}

} // namespace Serein::Interface
