#include "serein/compose/spacing.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <iostream>

TEST_CASE("Spacing") {
	using Serein::Compose::InsertChineseLatinSpacing;
	const auto input = QString::fromUtf8("中文abc");
	const auto plain = InsertChineseLatinSpacing(input,
		std::vector<int>(input.size() + 1));
	Require(plain.text == QString::fromUtf8("中文 abc"),
		"Han-Latin boundary not spaced");
	Require(plain.before[2] == 2 && plain.after[2] == 3
		&& plain.before[5] == 6, "inserted space offset mapping");

	const auto linked = QString::fromUtf8("中abc文");
	auto boundaries = std::vector<int>(linked.size() + 1);
	++boundaries[2];
	--boundaries[4];
	const auto protectedText = InsertChineseLatinSpacing(linked, boundaries);
	Require(protectedText.text == QString::fromUtf8("中 abc 文"),
		"spacing changed protected link text");
	const auto linkStart = protectedText.after[1];
	const auto linkEnd = protectedText.before[4];
	Require(protectedText.text.mid(linkStart, linkEnd - linkStart)
		== QString::fromLatin1("abc"), "protected entity span shifted");

	const auto already = QString::fromUtf8("中 abc 文");
	Require(InsertChineseLatinSpacing(already,
		std::vector<int>(already.size() + 1)).text == already,
		"spacing is not idempotent");
	std::cout << "PASS: Serein spacing and offsets" << std::endl;
}
