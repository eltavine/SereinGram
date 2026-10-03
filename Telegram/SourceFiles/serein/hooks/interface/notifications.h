#pragma once

#include <crl/crl_time.h>
#include <gsl/pointers>

#include <QtCore/QRect>

#include <optional>

class HistoryItem;

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

// WHY: nothing keeps upstream in charge of muted and unknown chats; true
// shows and false skips only when a quiet period or keyword alert applies.
[[nodiscard]] std::optional<bool> ReviewNotification(
	gsl::not_null<HistoryItem*> item,
	bool message);

} // namespace Serein::Interface
