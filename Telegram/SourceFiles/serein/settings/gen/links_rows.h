// Generated from proto/serein/settings/v1/links.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/links.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Links {

inline const auto kToggleRows = std::array<ToggleRow, 1>{ {
	{
		.option = &kStripTrackingParameters,
		.title = tr::lng_serein_strip_tracking_parameters,
		.id = u"serein/links/strip-tracking-parameters"_q,
		.keywords = {  },
		.icon = &st::menuIconLink,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_strip_tracking_parameters_about,
	},
} };

struct CustomRows {
	CustomRow linkRules;
	CustomRow previewLinkRules;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.linkRules();
	custom.previewLinkRules();
	AddToggle(builder, kToggleRows[0]);
}

} // namespace Serein::Links
