#include "serein/features/ghost/model/exceptions.h"
#include "serein/schema/gen/settings/ghost.h"

#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("GhostExceptions") {
	using namespace Serein::Ghost;
	auto value = QString();
	value = ToggleException(value, 42);
	value = ToggleException(value, 9000000000ULL);
	Require(value == u"42,9000000000"_q, "exceptions are not appended");
	Require(HasException(value, 42) && !HasException(value, 43),
		"exception lookup");
	value = ToggleException(value, 42);
	Require(value == u"9000000000"_q, "exception is not removed");
	Require(ToggleException(QString(), 0).isEmpty(), "zero peer accepted");
	Require(ParseExceptions(u"1,1"_q).empty()
		&& ParseExceptions(u"01"_q).empty()
		&& ParseExceptions(u"x"_q).empty()
		&& ParseExceptions(u"-1"_q).empty(),
		"malformed exceptions accepted");
	Require(kReadReceiptExceptions.validate(u"5,7"_q)
		&& !kReadReceiptExceptions.validate(u"5,5"_q)
		&& kReadReceiptExceptions.validate(QString()),
		"exception validator");
	auto many = QString();
	for (auto i = 1; i <= kReadExceptionsLimit + 1; ++i) {
		many = ToggleException(many, quint64(i));
	}
	Require(ParseExceptions(many).size() == std::size_t(kReadExceptionsLimit),
		"exception limit");
}
