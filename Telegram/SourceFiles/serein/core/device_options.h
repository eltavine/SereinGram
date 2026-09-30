#pragma once

#include "serein/core/options.h"

namespace Serein::details {

[[nodiscard]] inline Options &SharedDeviceOptions(RawPrefs &prefs) {
	static auto options = Options(prefs);
	return options;
}

} // namespace Serein::details
