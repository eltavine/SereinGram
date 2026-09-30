#pragma once

#include <QtCore/QString>

namespace Serein::Compose {

struct MentionQuery {
	quint64 userId = 0;
	QString username;
};

[[nodiscard]] MentionQuery ParseMentionQuery(const QString &input);

} // namespace Serein::Compose
