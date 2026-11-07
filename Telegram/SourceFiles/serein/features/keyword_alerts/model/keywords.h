#pragma once

#include "serein/schema/gen/config/notifications.h"

namespace Serein::Notifications {

inline constexpr auto kMaxKeywordRules = 50;
inline constexpr auto kMaxKeywordLength = 256;

[[nodiscard]] std::optional<KeywordAlerts> ReadKeywordAlerts(
	const QByteArray &raw);

[[nodiscard]] std::vector<KeywordRule> ParseKeywordLines(const QString &text);
[[nodiscard]] QString FormatKeywordLines(
	const std::vector<KeywordRule> &rules);
[[nodiscard]] bool ValidKeywordRule(const KeywordRule &rule);

[[nodiscard]] bool Matches(
	const KeywordAlerts &config,
	const QString &text,
	bool channel);

} // namespace Serein::Notifications
