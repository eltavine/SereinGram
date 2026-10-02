#include "serein/chats/reading_positions.h"
#include "serein/chats/options.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("ReadingPositions") {
	using namespace Serein::Chats;
	auto value = QString();
	value = SetReadingPosition(value, 7, 100);
	value = SetReadingPosition(value, 9, 200);
	value = SetReadingPosition(value, 7, 150);
	Require(value == u"7:150,9:200"_q, "reading position not moved to front");
	Require(FindReadingPosition(value, 7) == 150
		&& FindReadingPosition(value, 9) == 200
		&& FindReadingPosition(value, 11) == 0,
		"reading position lookup");
	value = SetReadingPosition(value, 7, 0);
	Require(value == u"9:200"_q, "reading position not removed");
	for (auto peer = quint64(1000); peer != 1120; ++peer) {
		value = SetReadingPosition(value, peer, 5);
	}
	Require(value.split(u',').size() == kReadingPositionsLimit
		&& FindReadingPosition(value, 1119) == 5
		&& FindReadingPosition(value, 9) == 0,
		"reading positions not capped");
	Require(ValidReadingPositions(QString()) && ValidReadingPositions(value),
		"valid reading positions rejected");
	for (const auto bad : {
			"7", "7:", ":5", "0:5", "7:0", "7:-3", "07:5", "7:05",
			"7:5,7:6", "7:5,", "a:b" }) {
		Require(!ValidReadingPositions(QString::fromLatin1(bad)),
			"malformed reading positions accepted");
	}
	Require(FindReadingPosition(u"7:5,7:6"_q, 7) == 0,
		"malformed reading positions used");
}
