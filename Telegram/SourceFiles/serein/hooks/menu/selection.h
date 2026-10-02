#pragma once

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

class HistoryInner;
class HistoryItem;
namespace HistoryView {
class ListWidget;
} // namespace HistoryView

namespace Serein::Menu {

class Selection final {
public:
	static void Select(HistoryInner *widget, HistoryItem *source);
	static void Select(HistoryView::ListWidget *widget, HistoryItem *source);
	static void SelectRange(HistoryInner *widget);
	static void SelectRange(HistoryView::ListWidget *widget);

};

class SelectionTarget final {
public:
	SelectionTarget(HistoryInner *widget);
	SelectionTarget(HistoryView::ListWidget *widget);

	[[nodiscard]] explicit operator bool() const;
	void selectSender(HistoryItem *source) const;
	void selectRange() const;

private:
	QPointer<QWidget> _widget;
	HistoryInner *_inner = nullptr;
	HistoryView::ListWidget *_list = nullptr;

};

} // namespace Serein::Menu
