#pragma once

#include <crl/crl_time.h>

namespace Nagram::Interface {

[[nodiscard]] crl::time NotificationDelay(
	crl::time upstream,
	crl::time minimum,
	bool otherDeviceActive);
[[nodiscard]] int AppIconBadge(int unread);

} // namespace Nagram::Interface
