#include "serein/hooks/interface/notifications.h"

#include "serein/interface/options.h"

#include <algorithm>

namespace Serein::Interface {

crl::time NotificationDelay(
		crl::time upstream,
		crl::time minimum,
		bool otherDeviceActive) {
	const auto override = ForDevice().Get(otherDeviceActive
		? kOtherDeviceNotificationDelay
		: kNotificationDelay);
	return std::max(override ? crl::time(override) : upstream, minimum);
}

int AppIconBadge(int unread) {
	return ForDevice().Get(kHideAppIconBadge) ? 0 : unread;
}

int NotificationLeft(QRect area, int width, bool top, int upstream) {
	return (top && ForDevice().Get(kCenterTopNotifications))
		? (area.x() + (area.width() - width) / 2)
		: upstream;
}

} // namespace Serein::Interface
