#pragma once

#include "base/assertion.h"
#include "base/basic_types.h"
#include "ui/text/text_entity.h"
#include "serein/core/options.h"
#include "serein/schema/gen/config/filters.h"
#include "serein/schema/gen/settings/filters.h"

#include <QtCore/QByteArray>

#include <optional>
#include <vector>

namespace Serein::Filters {

struct Result {
	TextWithEntities text;
	bool hidden = false;
	QString error;
	int matches = 0;
};

[[nodiscard]] std::optional<FilterRules> ReadRules(const QByteArray &raw);
[[nodiscard]] std::optional<std::vector<FilterRule>> ReadRuleList(
	const QByteArray &raw);
[[nodiscard]] QByteArray WriteRuleList(std::vector<FilterRule> rules);
[[nodiscard]] bool Validate(const QByteArray &raw);
[[nodiscard]] bool ValidRuleList(const QByteArray &raw);
[[nodiscard]] Result Apply(
	const QByteArray &raw,
	const TextWithEntities &source,
	const QString &author,
	const QString &peer,
	bool blocked,
	bool outgoing,
	const QString &searchable = QString(),
	const std::vector<FilterRule> &shared = {},
	const QString &topic = QString());

} // namespace Serein::Filters
