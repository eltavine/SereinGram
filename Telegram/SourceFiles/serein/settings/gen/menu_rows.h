// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/menu.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Menu {


struct CustomRows {
	CustomRow messageMenu;
	CustomRow confirmRepeat;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.messageMenu();
	custom.confirmRepeat();
}

} // namespace Serein::Menu
