#pragma once

#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Serein::Filters {

inline constexpr auto kHiddenMessagesLimit = 1000;

[[nodiscard]] QString HiddenMessageToken(quint64 peer, qint64 msg);
[[nodiscard]] QSet<QString> HiddenMessageSet(const QString &raw);
[[nodiscard]] QString ToggleHiddenMessages(
	const QString &raw,
	const QStringList &tokens);

} // namespace Serein::Filters
