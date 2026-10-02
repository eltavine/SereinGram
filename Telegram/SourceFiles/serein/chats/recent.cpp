#include "serein/chats/recent.h"

#include "serein/chats/options.h"
#include "serein/core/id_list.h"

#include <QtCore/QStringList>

#include <algorithm>

namespace Serein::Chats {

std::vector<quint64> ParseRecentChats(const QString &value) {
	return ParseIdList(value, kRecentChatsLimit).value_or(
		std::vector<quint64>());
}

bool ValidRecentChats(const QString &value) {
	return value.isEmpty() || !ParseRecentChats(value).empty();
}

QString PushRecentChat(const QString &value, quint64 id) {
	auto ids = ParseRecentChats(value);
	ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
	ids.insert(ids.begin(), id);
	if (int(ids.size()) > kRecentChatsLimit) {
		ids.resize(kRecentChatsLimit);
	}
	auto parts = QStringList();
	for (const auto entry : ids) {
		parts.push_back(QString::number(entry));
	}
	return parts.join(u',');
}

} // namespace Serein::Chats
