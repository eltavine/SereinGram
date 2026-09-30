#include "serein/schema/gen/config/sticker_catalog.h"

#include <QtCore/QSet>

#include <algorithm>

namespace Serein::MediaSchema {
namespace {

constexpr auto kMaximumTitleLength = 256;

[[nodiscard]] bool ValidTitle(const QString &title) {
	return title.size() <= kMaximumTitleLength
		&& QString::fromUtf8(title.toUtf8()) == title
		&& std::none_of(title.begin(), title.end(), [](QChar ch) {
			return ch.category() == QChar::Other_Control
				|| ch.category() == QChar::Separator_Line
				|| ch.category() == QChar::Separator_Paragraph;
		});
}

} // namespace

bool ValidStickerCatalogFile(const StickerCatalogFile &value) {
	auto seen = QSet<QString>();
	for (const auto &set : value.sets) {
		const auto key = set.shortName.toLower();
		if (!ValidTitle(set.title) || seen.contains(key)) {
			return false;
		}
		seen.insert(key);
	}
	return true;
}

} // namespace Serein::MediaSchema
