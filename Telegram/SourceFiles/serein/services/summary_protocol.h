#pragma once

#include "serein/hooks/services/model.h"

#include <QtCore/QJsonDocument>

#include <optional>
#include <vector>

namespace Serein {

inline constexpr auto kSummaryMessagesLimit = 60;
inline constexpr auto kSummaryLineLimit = 600;

struct SummaryLine {
	QString author;
	QString text;
};

[[nodiscard]] bool SupportsSummary(const ServiceDefinition &service);
[[nodiscard]] QJsonDocument BuildSummaryCall(
	const ServiceDefinition &service,
	const std::vector<SummaryLine> &lines,
	const QString &language);
[[nodiscard]] std::optional<QString> ParseSummaryResponse(
	const ServiceDefinition &service,
	const QByteArray &body);

} // namespace Serein
