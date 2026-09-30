#pragma once

class HistoryItem;
namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }

namespace Serein::Menu {

void InsertReadingAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

} // namespace Serein::Menu
