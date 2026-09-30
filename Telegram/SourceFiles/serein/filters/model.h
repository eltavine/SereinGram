#pragma once

#include "base/assertion.h"
#include "base/basic_types.h"
#include "ui/text/text_entity.h"
#include "serein/core/options.h"
#include "serein/schema/gen/config/filters.h"

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>

namespace Serein::Filters {


struct Result {
	TextWithEntities text;
	bool hidden = false;
	QString error;
	int matches = 0;
};

[[nodiscard]] QJsonObject Defaults();
[[nodiscard]] bool Validate(const QByteArray &raw);
[[nodiscard]] Result Apply(
	const QByteArray &raw,
	const TextWithEntities &source,
	const QString &author,
	const QString &peer,
	bool blocked,
	bool outgoing,
	const QString &searchable = QString());

inline const auto kRules = Option<QByteArray>{
	"serein.filters", Scope::Account, QByteArray(),
	Category::Rules, "lng_serein_filter_rules",
	static_cast<unsigned>(Flag::RefreshMessageView), Validate };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kRules));
}

} // namespace Serein::Filters
