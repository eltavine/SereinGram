#include "serein/menu/selection.h"

#include "history/history_inner_widget.h"
#include "history/history_widget.h"
#include "history/view/history_view_list_widget.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "window/window_session_controller.h"

namespace Serein::Menu {

void Selection::Select(HistoryInner *widget, HistoryItem *source) {
	if (!widget || !source || widget->hasSelectRestriction()) {
		return;
	}
	auto selected = widget->_selected;
	for (const auto history : { widget->_migrated, widget->_history.get() }) {
		if (!history) {
			continue;
		}
		for (const auto &block : history->blocks) {
			for (const auto &view : block->messages) {
				const auto item = view->data();
				if (item->from() != source->from() || !item->canBeSelected()) {
					continue;
				}
				if (!selected.contains(item) && selected.size() >= MaxSelectedItems) {
					widget->_controller->showToast(
						tr::lng_serein_menu_selection_limit(tr::now));
					return;
				}
				widget->changeSelection(&selected, item,
					HistoryInner::SelectAction::Select);
			}
		}
	}
	widget->clearTextSelection();
	widget->_selected = std::move(selected);
	widget->_accessibilitySelectionAnchor = nullptr;
	widget->update();
	widget->_widget->updateTopBarSelection();
}

void Selection::Select(HistoryView::ListWidget *widget, HistoryItem *source) {
	if (!widget || !source || widget->hasSelectRestriction()) {
		return;
	}
	auto selected = widget->_selected;
	for (const auto &view : widget->_items) {
		const auto item = view->data();
		if (item->from() != source->from()
			|| !widget->_delegate->listIsItemGoodForSelection(item)) {
			continue;
		}
		if (!selected.contains(item->fullId())
			&& selected.size() >= MaxSelectedItems) {
			widget->controller()->showToast(
				tr::lng_serein_menu_selection_limit(tr::now));
			return;
		}
		widget->changeSelection(selected, item,
			HistoryView::ListWidget::SelectAction::Select);
	}
	widget->clearTextSelection();
	widget->_selected = std::move(selected);
	widget->_accessibilitySelectionAnchor = nullptr;
	widget->pushSelectedItems();
	widget->update();
}

} // namespace Serein::Menu
