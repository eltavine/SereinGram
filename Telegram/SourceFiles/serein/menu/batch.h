#pragma once

#include "data/data_types.h"
#include "base/basic_types.h"
#include "serein/hooks/menu/selection.h"

class HistoryItem;
namespace Ui {
class PopupMenu;
} // namespace Ui
namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Menu {

void InsertBatchActions(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller,
	MessageIdsList selected,
	SelectionTarget selection);

} // namespace Serein::Menu
