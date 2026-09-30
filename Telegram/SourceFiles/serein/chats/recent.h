#pragma once

#include <QtCore/QString>

#include <vector>

namespace Serein::Chats {

inline constexpr auto kRecentChatsLimit = 30;

[[nodiscard]] std::vector<quint64> ParseRecentChats(const QString &value);
[[nodiscard]] QString PushRecentChat(const QString &value, quint64 id);

} // namespace Serein::Chats
