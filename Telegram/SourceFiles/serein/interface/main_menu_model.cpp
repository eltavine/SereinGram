#include "serein/interface/main_menu.h"
#include "base/basic_types.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

#include <algorithm>

namespace Serein::Interface {

QJsonObject MainMenuDefaults() {
	return {
		{ u"version"_q, 1 },
		{ u"order"_q, QJsonArray() },
		{ u"hidden"_q, QJsonArray() },
		{ u"title"_q, QString() },
		{ u"seasonalDecorations"_q, true },
	};
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

bool ValidMainMenu(const QJsonObject &value) {
	return ValidMainMenuBytes(
		QJsonDocument(value).toJson(QJsonDocument::Compact));
}

bool ValidMainMenuBytes(const QByteArray &value) {
	return value.isEmpty() || ParseMainMenuConfig(value).has_value();
}

} // namespace Serein::Interface
