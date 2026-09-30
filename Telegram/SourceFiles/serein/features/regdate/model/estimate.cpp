#include "serein/features/regdate/model/estimate.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

#include <algorithm>
#include <cmath>

namespace Serein::RegistrationDate {

std::vector<Point> ParsePoints(const QByteArray &json) {
	auto result = std::vector<Point>();
	for (const auto &entry : QJsonDocument::fromJson(json).array()) {
		const auto pair = entry.toArray();
		const auto id = pair.at(0).toDouble(-1);
		const auto date = QDate::fromString(
			pair.at(1).toString(),
			Qt::ISODate);
		if (pair.size() != 2 || id < 0 || id != std::floor(id)
			|| !date.isValid()) {
			return {};
		}
		result.push_back({ quint64(id), date.toJulianDay() });
	}
	std::sort(result.begin(), result.end(), [](Point a, Point b) {
		return a.id < b.id;
	});
	return result;
}

std::optional<Estimate> EstimateFor(
		const std::vector<Point> &points,
		quint64 userId) {
	if (points.empty()) {
		return std::nullopt;
	} else if (userId >= points.back().id) {
		return Estimate{
			.date = QDate::fromJulianDay(points.back().day),
			.lowerBound = (userId > points.back().id),
		};
	}
	const auto upper = std::upper_bound(
		points.begin(),
		points.end(),
		userId,
		[](quint64 id, Point point) { return id < point.id; });
	const auto lower = std::prev(upper);
	const auto share = double(userId - lower->id)
		/ double(upper->id - lower->id);
	const auto day = double(lower->day)
		+ share * double(upper->day - lower->day);
	return Estimate{ .date = QDate::fromJulianDay(qint64(std::llround(day))) };
}

} // namespace Serein::RegistrationDate
