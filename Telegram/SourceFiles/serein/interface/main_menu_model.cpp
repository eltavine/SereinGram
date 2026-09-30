#include "serein/hooks/interface/main_menu.h"
#include "base/basic_types.h"

#include <algorithm>

namespace Serein::Interface {

MainMenuConfig MainMenuDefaults() {
	auto result = MainMenuConfig();
	result.seasonalDecorations = true;
	return result;
}

bool ValidMainMenuTitle(const MainMenuConfig &value) {
	const auto &title = value.title;
	return title.size() <= 96
		&& QString::fromUtf8(title.toUtf8()) == title
		&& std::none_of(title.begin(), title.end(), [](QChar ch) {
			return ch.category() == QChar::Other_Control
				|| ch.category() == QChar::Separator_Line
				|| ch.category() == QChar::Separator_Paragraph;
		});
}

bool ValidMainMenuBytes(const QByteArray &value) {
	return value.isEmpty() || ParseMainMenuConfig(value).has_value();
}

} // namespace Serein::Interface
