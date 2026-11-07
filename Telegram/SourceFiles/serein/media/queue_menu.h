#pragma once

#include "data/data_types.h"

namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }
class HistoryItem;

namespace Serein::Media {

void InsertQueueAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller,
	MessageIdsList selected);

} // namespace Serein::Media
