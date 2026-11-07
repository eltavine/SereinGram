#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>

namespace Serein {

struct AyuMigration {
	QByteArray settings;
	QStringList skippedKeys;
};

[[nodiscard]] AyuMigration MigrateAyuSettings(
	const QJsonObject &source,
	quint64 userId);

} // namespace Serein
