#include "serein/media/download_names.h"

#include "base/basic_types.h"

#include <QtCore/QStringList>

namespace Serein::Media {
namespace {

constexpr auto kMaxFolderName = 64;

[[nodiscard]] bool Reserved(const QString &name) {
	static const auto names = QStringList{
		u"CON"_q, u"PRN"_q, u"AUX"_q, u"NUL"_q,
		u"COM1"_q, u"COM2"_q, u"COM3"_q, u"COM4"_q, u"COM5"_q,
		u"COM6"_q, u"COM7"_q, u"COM8"_q, u"COM9"_q,
		u"LPT1"_q, u"LPT2"_q, u"LPT3"_q, u"LPT4"_q, u"LPT5"_q,
		u"LPT6"_q, u"LPT7"_q, u"LPT8"_q, u"LPT9"_q,
	};
	return names.contains(name.section(u'.', 0, 0), Qt::CaseInsensitive);
}

} // namespace

QString DownloadFolderName(const QString &name, quint64 id) {
	static const auto forbidden = u"<>:\"/\\|?*"_q;
	auto result = QString();
	result.reserve(name.size());
	for (const auto ch : name) {
		result += (ch.unicode() < 32 || forbidden.contains(ch)) ? u'_' : ch;
	}
	result = result.left(kMaxFolderName).trimmed();
	while (result.endsWith(u'.')) {
		result.chop(1);
	}
	result = result.trimmed();
	if (result.isEmpty()) {
		return QString::number(id);
	}
	return Reserved(result) ? (result + u'_') : result;
}

} // namespace Serein::Media
