#pragma once

#include "base/basic_types.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace Serein::Display {

void SaveJsonFile(
	const QString &name,
	QByteArray bytes,
	Fn<void(QString text)> notify);
void OpenJsonFile(
	qint64 maximumBytes,
	Fn<void(QByteArray bytes)> done,
	Fn<void(QString text)> notify);

} // namespace Serein::Display
