#pragma once

class HistoryItem;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Menu {

void InsertEditHistoryAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

void InsertDeletedMessagesAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

} // namespace Serein::Menu
