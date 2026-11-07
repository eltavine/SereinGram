#pragma once

#include <QtCore/QString>

#include <optional>

namespace Serein::Jump {

struct Target {
	qint64 messageId = 0;
	qint64 channelId = 0;
	QString username;
};

[[nodiscard]] std::optional<Target> ParseTarget(QStringView input);

} // namespace Serein::Jump
