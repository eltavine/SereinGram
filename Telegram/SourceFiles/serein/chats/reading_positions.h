#pragma once

#include <QtCore/QString>

namespace Serein::Chats {

inline constexpr auto kReadingPositionsLimit = 100;

[[nodiscard]] qint64 FindReadingPosition(const QString &value, quint64 peer);
[[nodiscard]] QString SetReadingPosition(
	const QString &value,
	quint64 peer,
	qint64 message);

} // namespace Serein::Chats
