// Generated from proto/serein/settings/v1/links.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/links.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Links {


struct CustomRows {
	CustomRow linkRules;
	CustomRow previewLinkRules;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.linkRules();
	custom.previewLinkRules();
}

} // namespace Serein::Links
