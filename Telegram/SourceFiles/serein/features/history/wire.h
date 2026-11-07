#pragma once

#include <QtCore/QByteArray>

#include <optional>

namespace Serein::HistoryFeature {

[[nodiscard]] int WireLayer();
[[nodiscard]] QByteArray SerializeMessage(const MTPMessage &message);
[[nodiscard]] std::optional<MTPMessage> ParseMessage(const QByteArray &bytes);

} // namespace Serein::HistoryFeature
