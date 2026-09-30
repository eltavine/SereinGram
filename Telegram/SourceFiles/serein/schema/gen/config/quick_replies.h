// Generated from proto/serein/config/v1/quick_replies.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Compose {

struct QuickReplies {
	std::vector<QString> replies;

	friend bool operator==(const QuickReplies &, const QuickReplies &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	QuickReplies &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const QuickReplies &value);
[[nodiscard]] bool Validate(
	const QuickReplies &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] std::optional<QuickReplies> ParseQuickReplies(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeQuickReplies(const QuickReplies &value);

} // namespace Serein::Compose
