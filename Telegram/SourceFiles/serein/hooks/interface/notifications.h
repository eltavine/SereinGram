#pragma once

#include <crl/crl_time.h>

#include <QtCore/QRect>

namespace Serein::Interface {

[[nodiscard]] crl::time NotificationDelay(
	crl::time upstream,
	crl::time minimum,
	bool otherDeviceActive);
[[nodiscard]] int AppIconBadge(int unread);
[[nodiscard]] int NotificationLeft(
	QRect area,
	int width,
	bool top,
	int upstream);

} // namespace Serein::Interface
