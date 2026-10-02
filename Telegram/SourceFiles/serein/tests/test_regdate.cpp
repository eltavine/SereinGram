#include "serein/features/regdate/model/estimate.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QFile>

TEST_CASE("RegistrationDate") {
	using namespace Serein::RegistrationDate;
	const auto points = ParsePoints(
		R"([[200,"2020-01-11"],[0,"2020-01-01"],[100,"2020-01-03"]])");
	Require(points.size() == 3 && points.front().id == 0,
		"registration points not parsed and sorted");
	const auto exact = EstimateFor(points, 100);
	Require(exact && exact->date == QDate(2020, 1, 3) && !exact->lowerBound,
		"known point not returned");
	const auto middle = EstimateFor(points, 150);
	Require(middle && middle->date == QDate(2020, 1, 7),
		"registration date not interpolated");
	const auto beyond = EstimateFor(points, 500);
	Require(beyond && beyond->date == QDate(2020, 1, 11) && beyond->lowerBound,
		"id after the data not marked as a lower bound");
	Require(!EstimateFor({}, 1), "estimate without data");
	Require(ParsePoints(R"([[1,"not a date"]])").empty()
		&& ParsePoints(R"([[1.5,"2020-01-01"]])").empty()
		&& ParsePoints("{broken").empty(),
		"malformed registration points accepted");

	auto file = QFile(QString::fromUtf8(SEREIN_REGDATE_POINTS));
	Require(file.open(QIODevice::ReadOnly), "bundled registration points");
	const auto bundled = ParsePoints(file.readAll());
	Require(bundled.size() >= 200, "bundled registration points not parsed");
	const auto early = EstimateFor(bundled, 2768409);
	Require(early && early->date == QDate(2013, 11, 1),
		"bundled data point not matched");
}
