#pragma once

#include "base/assertion.h"
#include "base/basic_types.h"
#include "ui/text/text_entity.h"
#include "nagram/core/options.h"

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>

namespace Nagram::Filters {


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
	"nagram.filters", Scope::Account, QByteArray(),
	Category::Rules, "lng_nagram_filter_rules",
	static_cast<unsigned>(Flag::RefreshMessageView), Validate };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kRules));
}

} // namespace Nagram::Filters
