#pragma once

#include "serein/schema/gen/config/notifications.h"

namespace Serein::Notifications {

struct NotificationFacts {
	bool message = false;
	bool fromContact = false;
	bool pinnedChat = false;
	bool mentionsMe = false;
	bool keywordMatch = false;
};

inline constexpr auto kMinutesPerDay = 24 * 60;

[[nodiscard]] std::optional<QuietHours> ReadQuietHours(const QByteArray &raw);

[[nodiscard]] bool Active(
	const QuietHours &config,
	int isoWeekday,
	int minuteOfDay);
[[nodiscard]] bool Silences(
	const QuietHours &config,
	const NotificationFacts &facts,
	int isoWeekday,
	int minuteOfDay);

} // namespace Serein::Notifications
