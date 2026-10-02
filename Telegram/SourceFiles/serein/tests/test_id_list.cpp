#include "serein/core/id_list.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

#include <vector>

TEST_CASE("IdList") {
	using Serein::ParseIdList;
	using Ids = std::vector<quint64>;
	Require(ParseIdList(QString(), 3) == Ids(), "empty list rejected");
	Require(ParseIdList(u"7,1,42"_q, 3) == Ids{ 7, 1, 42 },
		"order or values changed");
	Require(ParseIdList(u"18446744073709551615"_q, 1)
		== Ids{ 18446744073709551615ULL }, "largest id rejected");
	for (const auto &invalid : {
			u"1,1"_q,
			u"0"_q,
			u"01"_q,
			u"+1"_q,
			u"-1"_q,
			u" 1"_q,
			u"1,"_q,
			u",1"_q,
			u"1;2"_q,
			u"18446744073709551616"_q }) {
		Require(!ParseIdList(invalid, 3), "malformed list accepted");
	}
	Require(!ParseIdList(u"1,2,3,4"_q, 3), "list over the limit accepted");
	Require(ParseIdList(u"1,2,3"_q, 3).has_value(),
		"list at the limit rejected");
}
