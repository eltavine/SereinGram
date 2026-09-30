#pragma once

class HistoryItem;
namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }

namespace Serein::Filters {

void InsertAuthorAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

} // namespace Serein::Filters
