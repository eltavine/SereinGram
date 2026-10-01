#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <map>
#include <optional>

namespace Serein::Network {

inline constexpr auto kMaxProxyNotes = 200;
inline constexpr auto kMaxProxyNoteLength = 64;

using ProxyNotes = std::map<QString, QString>;

[[nodiscard]] QString ProxyNoteKey(const QString &host, quint32 port);
[[nodiscard]] bool ValidProxyNote(const QString &note);
[[nodiscard]] std::optional<ProxyNotes> ParseProxyNotes(const QByteArray &raw);
[[nodiscard]] QByteArray SerializeProxyNotes(const ProxyNotes &notes);

} // namespace Serein::Network
