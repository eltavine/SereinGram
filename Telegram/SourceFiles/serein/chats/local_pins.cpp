#include "serein/chats/local_pins.h"

#include "serein/chats/options.h"
#include "serein/core/id_list.h"

#include <QtCore/QStringList>

#include <algorithm>
#include <optional>

namespace Serein::Chats {
namespace {

[[nodiscard]] std::optional<std::vector<quint64>> Parse(const QString &value) {
	return ParseIdList(value, kLocalPinsLimit);
}

} // namespace

bool ValidLocalPins(const QString &value) {
	return Parse(value).has_value();
}

std::vector<quint64> ParseLocalPins(const QString &value) {
	return Parse(value).value_or(std::vector<quint64>());
}

QString ToggleLocalPin(const QString &value, quint64 id) {
	auto ids = ParseLocalPins(value);
	const auto i = std::find(ids.begin(), ids.end(), id);
	if (i != ids.end()) {
		ids.erase(i);
	} else if (id && int(ids.size()) < kLocalPinsLimit) {
		ids.push_back(id);
	}
	auto parts = QStringList();
	for (const auto entry : ids) {
		parts.push_back(QString::number(entry));
	}
	return parts.join(u',');
}

} // namespace Serein::Chats
