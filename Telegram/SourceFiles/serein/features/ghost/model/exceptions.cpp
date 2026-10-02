#include "serein/features/ghost/model/exceptions.h"

#include "serein/core/id_list.h"

#include <QtCore/QStringList>

#include <algorithm>
#include <optional>

namespace Serein::Ghost {
namespace {

[[nodiscard]] std::optional<std::vector<quint64>> Parse(const QString &value) {
	return ParseIdList(value, kReadExceptionsLimit);
}

} // namespace

std::vector<quint64> ParseExceptions(const QString &value) {
	return Parse(value).value_or(std::vector<quint64>());
}

bool HasException(const QString &value, quint64 peerId) {
	const auto ids = ParseExceptions(value);
	return std::find(ids.begin(), ids.end(), peerId) != ids.end();
}

QString ToggleException(const QString &value, quint64 peerId) {
	auto ids = ParseExceptions(value);
	const auto i = std::find(ids.begin(), ids.end(), peerId);
	if (i != ids.end()) {
		ids.erase(i);
	} else if (peerId && int(ids.size()) < kReadExceptionsLimit) {
		ids.push_back(peerId);
	}
	auto parts = QStringList();
	for (const auto id : ids) {
		parts.push_back(QString::number(id));
	}
	return parts.join(u',');
}

bool ValidReadReceiptExceptions(const QString &value) {
	return Parse(value).has_value();
}

} // namespace Serein::Ghost
