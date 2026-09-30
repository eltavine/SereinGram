#pragma once

#include "serein/schema/gen/settings/privacy.h"

namespace Serein::Privacy {

[[nodiscard]] inline bool DemoMode() {
	return ForDevice().Get(kDemoMode);
}

} // namespace Serein::Privacy
