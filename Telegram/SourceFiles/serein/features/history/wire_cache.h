#pragma once

#include <QtCore/QByteArray>
#include <gsl/pointers>

class HistoryItem;

namespace Main {
class Session;
} // namespace Main

namespace Serein::HistoryFeature {

void StartWireCapture(gsl::not_null<Main::Session*> session);
[[nodiscard]] QByteArray CapturedWire(gsl::not_null<const HistoryItem*> item);

} // namespace Serein::HistoryFeature
