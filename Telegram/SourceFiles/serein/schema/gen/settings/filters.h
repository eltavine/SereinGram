// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Filters {

[[nodiscard]] bool Validate(const QByteArray &value);

inline const auto kRules = Option<QByteArray>{
	"serein.filters",
	Scope::Account,
	QByteArray(),
	Category::Rules,
	"lng_serein_filter_rules",
	static_cast<unsigned>(Flag::RefreshMessageView),
	&Validate };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kRules));
}

} // namespace Serein::Filters
