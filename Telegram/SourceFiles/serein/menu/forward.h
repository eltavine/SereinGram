#pragma once

namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }
class HistoryItem;

namespace Serein::Menu {

void InsertForwardActions(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

} // namespace Serein::Menu
