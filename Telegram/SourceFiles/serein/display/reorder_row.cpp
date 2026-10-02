#include "serein/display/reorder_row.h"

#include "styles/style_settings.h"

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>

namespace Serein::Display {

ReorderRow::ReorderRow(
	QWidget *parent,
	rpl::producer<QString> title,
	QString mime,
	QByteArray id,
	Fn<void(const QByteArray &from, const QByteArray &to)> moved)
: Ui::SettingsButton(parent, std::move(title), st::settingsButtonNoIcon)
, _mime(std::move(mime))
, _id(std::move(id))
, _moved(std::move(moved)) {
	setAcceptDrops(true);
}

void ReorderRow::mousePressEvent(QMouseEvent *event) {
	_dragStart = event->globalPosition().toPoint();
	Ui::SettingsButton::mousePressEvent(event);
}

void ReorderRow::mouseMoveEvent(QMouseEvent *event) {
	const auto distance = event->globalPosition().toPoint() - _dragStart;
	if ((event->buttons() & Qt::LeftButton)
		&& (distance.manhattanLength() >= QApplication::startDragDistance())) {
		const auto data = new QMimeData();
		data->setData(_mime, _id);
		const auto drag = new QDrag(this);
		drag->setMimeData(data);
		drag->exec(Qt::MoveAction);
		return;
	}
	Ui::SettingsButton::mouseMoveEvent(event);
}

void ReorderRow::dragEnterEvent(QDragEnterEvent *event) {
	if (event->mimeData()->hasFormat(_mime)) {
		event->acceptProposedAction();
	}
}

void ReorderRow::dropEvent(QDropEvent *event) {
	const auto from = event->mimeData()->data(_mime);
	if (!from.isEmpty() && from != _id) {
		_moved(from, _id);
	}
	event->acceptProposedAction();
}

} // namespace Serein::Display
