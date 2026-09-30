#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QDate>

#include <optional>
#include <vector>

namespace Serein::RegistrationDate {

struct Point {
	quint64 id = 0;
	qint64 day = 0;
};

struct Estimate {
	QDate date;
	bool lowerBound = false;
};

[[nodiscard]] std::vector<Point> ParsePoints(const QByteArray &json);
[[nodiscard]] std::optional<Estimate> EstimateFor(
	const std::vector<Point> &points,
	quint64 userId);

} // namespace Serein::RegistrationDate
