#pragma once

#include <QtCore/QStringList>

namespace Serein::Network {

inline constexpr auto kSubscriptionLinksLimit = 200;
inline constexpr auto kSubscriptionMaximumSize = 1024 * 1024;

[[nodiscard]] QStringList ExtractProxyLinks(const QString &body);

} // namespace Serein::Network
