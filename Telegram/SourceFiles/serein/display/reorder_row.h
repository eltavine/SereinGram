#pragma once

#include "ui/widgets/buttons.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace Serein::Display {

class ReorderRow final : public Ui::SettingsButton {
public:
	ReorderRow(
		QWidget *parent,
		rpl::producer<QString> title,
		QString mime,
		QByteArray id,
		Fn<void(const QByteArray &from, const QByteArray &to)> moved);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;

private:
	const QString _mime;
	const QByteArray _id;
	const Fn<void(const QByteArray &from, const QByteArray &to)> _moved;
	QPoint _dragStart;

};

} // namespace Serein::Display
