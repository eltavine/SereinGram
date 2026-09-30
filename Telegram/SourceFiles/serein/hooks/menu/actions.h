#pragma once

#include "base/basic_types.h"
#include "serein/hooks/menu/action_id.h"
#include "serein/hooks/menu/selection.h"

#include <vector>

class HistoryItem;
class QAction;
struct FullMsgId;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Menu {

[[nodiscard]] int DeleteActionIndex(Ui::PopupMenu *menu);
void Tag(QAction *action, ActionId id);
void Apply(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller,
	std::vector<FullMsgId> selected,
	SelectionTarget selection);

} // namespace Serein::Menu
