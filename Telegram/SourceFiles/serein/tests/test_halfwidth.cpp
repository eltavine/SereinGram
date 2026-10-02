#include "serein/hooks/interface/text.h"
#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestHalfwidthPunctuation() {
	using Serein::Interface::HalfwidthPunctuation;
	Require(HalfwidthPunctuation(u"ＡＢＣｘｙｚ１２３！？～"_q)
		== u"ABCxyz123!?~"_q, "fullwidth ASCII not converted");
	Require(HalfwidthPunctuation(u"你好，世界。"_q) == u"你好,世界."_q,
		"comma and full stop not converted");
	Require(HalfwidthPunctuation(u"甲、乙　丙"_q) == u"甲,乙 丙"_q,
		"enumeration comma or ideographic space not converted");
	Require(HalfwidthPunctuation(u"《书》【注】"_q) == u"<书>[注]"_q,
		"brackets not converted");
	Require(HalfwidthPunctuation(u"‘单’“双”"_q) == u"'单'\"双\""_q,
		"quotation marks not converted");
	Require(HalfwidthPunctuation(u"😀 ｆｉｎｅ"_q) == u"😀 fine"_q,
		"surrogate pairs changed");
	Require(HalfwidthPunctuation(u"\uFF00\uFF5F「」"_q)
		== u"\uFF00\uFF5F「」"_q, "characters outside the table changed");
	Require(HalfwidthPunctuation(QString()).isEmpty(),
		"empty text changed");
}
