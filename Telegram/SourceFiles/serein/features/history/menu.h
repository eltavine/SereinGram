#pragma once

class HistoryItem;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::HistoryFeature {

void InsertEditHistoryAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

void InsertDeletedMessagesAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

void InsertHistoryExclusionAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

void InsertRestoredMediaAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

} // namespace Serein::HistoryFeature
