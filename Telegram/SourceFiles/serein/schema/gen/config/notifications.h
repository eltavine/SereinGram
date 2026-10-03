// Generated from proto/serein/config/v1/notifications.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Notifications {

struct QuietHours {
	bool enabled = false;
	int startMinute = 0;
	int endMinute = 0;
	std::vector<int> weekdays;
	bool allowContacts = false;
	bool allowPinned = false;
	bool allowMentions = false;
	bool allowKeywords = false;

	friend bool operator==(const QuietHours &, const QuietHours &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	QuietHours &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const QuietHours &value);
[[nodiscard]] bool Validate(
	const QuietHours &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidQuietHours(const QuietHours &value);
[[nodiscard]] std::optional<QuietHours> ParseQuietHours(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeQuietHours(const QuietHours &value);

struct KeywordRule {
	QString pattern;
	bool regex = false;
	bool caseSensitive = false;

	friend bool operator==(const KeywordRule &, const KeywordRule &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	KeywordRule &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const KeywordRule &value);
[[nodiscard]] bool Validate(
	const KeywordRule &value,
	Codec::Error &error,
	const QString &path);

struct KeywordAlerts {
	bool enabled = false;
	std::vector<KeywordRule> rules;
	bool includeChannels = false;

	friend bool operator==(const KeywordAlerts &, const KeywordAlerts &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	KeywordAlerts &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const KeywordAlerts &value);
[[nodiscard]] bool Validate(
	const KeywordAlerts &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidKeywordAlerts(const KeywordAlerts &value);
[[nodiscard]] std::optional<KeywordAlerts> ParseKeywordAlerts(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeKeywordAlerts(const KeywordAlerts &value);

} // namespace Serein::Notifications
