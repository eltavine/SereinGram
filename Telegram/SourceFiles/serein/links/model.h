#pragma once

#include "serein/core/options.h"
#include "serein/schema/gen/config/links.h"
#include "serein/schema/gen/settings/links.h"

#include <QtCore/QJsonObject>
#include <QtCore/QUrl>

namespace Serein::Links {

struct Result {
	QUrl url;
	QString error;
	bool changed = false;
};

[[nodiscard]] QJsonObject Defaults();
[[nodiscard]] bool Validate(const QByteArray &raw);
[[nodiscard]] QJsonObject NewRule(
	const QString &host,
	const QString &replacementHost,
	const QStringList &removeParameters);
[[nodiscard]] Result Rewrite(const QByteArray &raw, const QString &original);

} // namespace Serein::Links
