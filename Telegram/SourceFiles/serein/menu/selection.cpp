#include "serein/hooks/menu/selection.h"

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

void Selection::SelectRange(HistoryInner *widget) {
	if (!widget
		|| widget->hasSelectRestriction()
		|| widget->_selected.size() < 2) {
		return;
	}
	auto items = std::vector<HistoryItem*>();
	for (const auto history : { widget->_migrated, widget->_history.get() }) {
		if (!history) {
			continue;
		}
		for (const auto &block : history->blocks) {
			for (const auto &view : block->messages) {
				items.push_back(view->data().get());
			}
		}
	}
	auto first = -1;
	auto last = -1;
	for (auto i = 0, count = int(items.size()); i != count; ++i) {
		if (widget->_selected.contains(items[i])) {
			first = (first < 0) ? i : first;
			last = i;
		}
	}
	if (first < 0 || last <= first) {
		return;
	}
	auto selected = widget->_selected;
	for (auto i = first; i <= last; ++i) {
		const auto item = items[i];
		if (!item->canBeSelected()) {
			continue;
		} else if (!selected.contains(item)
			&& selected.size() >= MaxSelectedItems) {
			widget->_controller->showToast(
				tr::lng_serein_menu_selection_limit(tr::now));
			return;
		}
		widget->changeSelection(&selected, item,
			HistoryInner::SelectAction::Select);
	}
	widget->clearTextSelection();
	widget->_selected = std::move(selected);
	widget->_accessibilitySelectionAnchor = nullptr;
	widget->update();
	widget->_widget->updateTopBarSelection();
}

void Selection::SelectRange(HistoryView::ListWidget *widget) {
	if (!widget
		|| widget->hasSelectRestriction()
		|| widget->_selected.size() < 2) {
		return;
	}
	auto first = -1;
	auto last = -1;
	for (auto i = 0, count = int(widget->_items.size()); i != count; ++i) {
		if (widget->_selected.contains(widget->_items[i]->data()->fullId())) {
			first = (first < 0) ? i : first;
			last = i;
		}
	}
	if (first < 0 || last <= first) {
		return;
	}
	auto selected = widget->_selected;
	for (auto i = first; i <= last; ++i) {
		const auto item = widget->_items[i]->data();
		if (!widget->_delegate->listIsItemGoodForSelection(item)) {
			continue;
		} else if (!selected.contains(item->fullId())
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

SelectionTarget::SelectionTarget(HistoryInner *widget)
: _widget(widget)
, _inner(widget) {
}

SelectionTarget::SelectionTarget(HistoryView::ListWidget *widget)
: _widget(widget)
, _list(widget) {
}

SelectionTarget::operator bool() const {
	return _widget != nullptr;
}

void SelectionTarget::selectSender(HistoryItem *source) const {
	if (!_widget) {
		return;
	} else if (_inner) {
		Selection::Select(_inner, source);
	} else {
		Selection::Select(_list, source);
	}
}

void SelectionTarget::selectRange() const {
	if (!_widget) {
		return;
	} else if (_inner) {
		Selection::SelectRange(_inner);
	} else {
		Selection::SelectRange(_list);
	}
}

} // namespace Serein::Menu
