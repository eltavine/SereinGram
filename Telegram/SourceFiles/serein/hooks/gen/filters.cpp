// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/filters.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/filters.h"

namespace Serein::Hooks::Filters {

QByteArray Rules(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Filters::kRules);
}

rpl::producer<QByteArray> RulesValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Filters::kRules);
}

} // namespace Serein::Hooks::Filters
