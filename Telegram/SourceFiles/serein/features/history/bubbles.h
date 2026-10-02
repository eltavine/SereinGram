#pragma once

#include "serein/ports/history_store.h"

#include <vector>

class PeerData;

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::HistoryFeature {

void ShowDeletedBubbles(
	not_null<Window::SessionController*> controller,
	not_null<PeerData*> peer,
	std::vector<History::Record> records);

} // namespace Serein::HistoryFeature
