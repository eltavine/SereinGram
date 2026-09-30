#pragma once

#include "serein/core/options.h"
#include "serein/schema/gen/config/links.h"
#include "serein/schema/gen/settings/links.h"

#include <QtCore/QUrl>

#include <optional>

namespace Serein::Links {

struct Result {
	QUrl url;
	QString error;
	bool changed = false;
};

[[nodiscard]] bool Validate(const QByteArray &raw);
[[nodiscard]] std::optional<LinkRules> ReadRules(const QByteArray &raw);
[[nodiscard]] QByteArray WriteRules(const LinkRules &rules);
[[nodiscard]] LinkRule NewRule();
[[nodiscard]] Result Rewrite(const LinkRules &rules, const QString &original);
[[nodiscard]] Result Rewrite(const QByteArray &raw, const QString &original);

} // namespace Serein::Links
