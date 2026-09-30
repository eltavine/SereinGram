#include "serein/compose/text_replacements.h"
#include "serein/compose/options.h"
#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestTextReplacements() {
	using namespace Serein::Compose;
	const auto parsed = ParseReplacementLines(
		u"  brb => be right back \n\n:shrug:=>¯\\_(ツ)_/¯\nomw =>"_q);
	Require(parsed
		&& parsed->rules.size() == 3
		&& parsed->rules[0] == TextReplacement{ u"brb"_q, u"be right back"_q }
		&& parsed->rules[1].to == u"¯\\_(ツ)_/¯"_q
		&& parsed->rules[2].to.isEmpty(),
		"replacement lines not parsed");
	Require(ParseReplacementLines(FormatReplacementLines(*parsed)) == parsed,
		"replacement lines do not round trip");
	Require(ParseReplacementLines(QString()) == TextReplacements(),
		"empty replacement text rejected");
	for (const auto bad : {
			"brb be right back",
			"=> nothing",
			"a => b\na => c",
			"thisisaverylongtriggerwordmorethan32 => x" }) {
		Require(!ParseReplacementLines(QString::fromUtf8(bad)),
			"malformed replacement lines accepted");
	}
	const auto raw = WriteTextReplacements(*parsed);
	Require(ReadTextReplacements(raw) == parsed
		&& WriteTextReplacements(TextReplacements()).isEmpty()
		&& ReadTextReplacements(QByteArray()) == TextReplacements()
		&& ValidTextReplacementsBytes(raw)
		&& !ValidTextReplacementsBytes("{\"version\":1,\"rules\":[{\"from\":\"a\",\"to\":\"b\"},{\"from\":\"a\",\"to\":\"c\"}]}"),
		"stored replacements mishandled");
}
