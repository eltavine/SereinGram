// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/filters.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Filters {


struct CustomRows {
	CustomRow filters;
	CustomRow ruleSubscription;
	CustomRow hiddenMessages;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.filters();
	custom.ruleSubscription();
	custom.hiddenMessages();
}

} // namespace Serein::Filters
