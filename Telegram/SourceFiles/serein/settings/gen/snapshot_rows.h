// Generated from proto/serein/settings/v1/snapshot.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/snapshot.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Snapshot {


struct CustomRows {
	CustomRow snapshot;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	custom.snapshot();
}

} // namespace Serein::Snapshot
