#include "serein/filters/hidden_messages.h"

#include "serein/schema/gen/settings/filters.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QStringList>

TEST_CASE("HiddenMessages") {
	using namespace Serein::Filters;
	const auto first = HiddenMessageToken(777, 15);
	const auto second = HiddenMessageToken(777, 16);
	Require(first == u"777:15"_q, "hidden message token format changed");
	Require(ValidHiddenMessages(QString()), "no hidden messages rejected");
	Require(ValidHiddenMessages(first + u',' + second),
		"two hidden messages rejected");
	for (const auto bad : { "777", "777:", ":15", "0:15", "777:0", "777:-1",
			"777:015", "a:15", "777:15,777:15", "777:15,", "777:15 " }) {
		Require(!ValidHiddenMessages(QString::fromLatin1(bad)),
			"invalid hidden messages accepted");
	}
	const auto album = QStringList{ first, second };
	const auto hidden = ToggleHiddenMessages(QString(), album);
	Require(hidden == first + u',' + second, "album was not hidden");
	Require(HiddenMessageSet(hidden).contains(second),
		"hidden message missing from the lookup set");
	Require(ToggleHiddenMessages(hidden, album).isEmpty(),
		"album was not shown again");
	auto many = QString();
	for (auto i = 1; i <= kHiddenMessagesLimit; ++i) {
		many = ToggleHiddenMessages(many, { HiddenMessageToken(1, i) });
	}
	many = ToggleHiddenMessages(many, { HiddenMessageToken(2, 1) });
	const auto set = HiddenMessageSet(many);
	Require(set.size() == kHiddenMessagesLimit
		&& !set.contains(HiddenMessageToken(1, 1))
		&& set.contains(HiddenMessageToken(2, 1)),
		"hidden messages limit does not drop the oldest entry");
	Require(ValidHiddenMessages(many), "trimmed hidden messages rejected");
}
