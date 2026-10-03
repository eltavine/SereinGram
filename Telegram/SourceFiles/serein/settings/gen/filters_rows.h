// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/filters.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"

#include <array>

namespace Serein::Filters {


struct CustomRows {
	CustomRow filters;
	CustomRow ruleSubscription;
	CustomRow hiddenMessages;
	CustomRow keywordAlerts;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.filters();
	custom.ruleSubscription();
	custom.hiddenMessages();
	custom.keywordAlerts();
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_rules;
inline const auto kSubpageIcon = &st::menuIconTagFilter;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	builder.addSectionButton({
		.title = (*kSubpageTitle)(),
		.targetSection = section,
		.icon = { kSubpageIcon },
		.keywords = { u"filter"_q, u"link"_q },
	});
}

} // namespace Serein::Filters
