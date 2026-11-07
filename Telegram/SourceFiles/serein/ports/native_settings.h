#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <functional>

namespace Serein {

struct NativeSettingsPlan {
	QStringList preview;
	QString error;
	std::function<bool()> unchanged;
	std::function<void()> apply;
};

} // namespace Serein
