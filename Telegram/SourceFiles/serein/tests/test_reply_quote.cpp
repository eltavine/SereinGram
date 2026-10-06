#include "serein/features/history/model/reply_quote.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

namespace {

using Serein::HistoryFeature::kReplyQuoteLimit;
using Serein::HistoryFeature::QuoteDeletedMessage;

[[nodiscard]] EntityInText Quote(int offset, int length) {
	return EntityInText(EntityType::Blockquote, offset, length, u"1"_q);
}

} // namespace

TEST_CASE("ReplyQuotePrependsCollapsedQuote") {
	const auto result = QuoteDeletedMessage(
		{ u"  deleted words\n\n"_q },
		{ u"my answer"_q, { EntityInText(EntityType::Bold, 3, 6) } });
	Require(
		result.text == u"deleted words\nmy answer"_q,
		"quote is trimmed and placed above the answer");
	Require(result.entities.size() == 2, "only the quote and the bold");
	Require(result.entities[0] == Quote(0, 13), "collapsed quote block");
	Require(
		result.entities[1] == EntityInText(EntityType::Bold, 17, 6),
		"answer formatting moves behind the quote");
}

TEST_CASE("ReplyQuoteKeepsOnlySafeFormatting") {
	const auto quote = TextWithEntities{
		u"@friend said *this* http://x.y"_q,
		{
			EntityInText(EntityType::Mention, 0, 7),
			EntityInText(EntityType::Italic, 13, 6),
			EntityInText(EntityType::CustomEmoji, 13, 1, u"5"_q),
			EntityInText(EntityType::Url, 20, 10),
		},
	};
	const auto result = QuoteDeletedMessage(quote, {});
	Require(result.text == quote.text, "quote alone without an answer");
	Require(result.entities.size() == 3, "mentions and custom emoji dropped");
	Require(result.entities[0] == Quote(0, 30), "quote block first");
	Require(
		result.entities[1] == EntityInText(EntityType::Italic, 13, 6),
		"formatting kept");
	Require(
		result.entities[2] == EntityInText(EntityType::Url, 20, 10),
		"whole links kept");
}

TEST_CASE("ReplyQuoteCutsLongMessages") {
	const auto source = QString(kReplyQuoteLimit + 10, QChar('a'));
	const auto quote = TextWithEntities{
		source,
		{
			EntityInText(EntityType::Bold, 0, int(source.size())),
			EntityInText(EntityType::Url, kReplyQuoteLimit - 2, 5),
		},
	};
	const auto result = QuoteDeletedMessage(quote, { u"ok"_q });
	const auto quoted = kReplyQuoteLimit + 1;
	Require(
		result.text == source.left(kReplyQuoteLimit)
			+ QChar(0x2026)
			+ u"\nok"_q,
		"quote cut at the limit with an ellipsis");
	Require(result.entities.size() == 2, "cut link dropped");
	Require(result.entities[0] == Quote(0, quoted), "quote covers the cut");
	Require(
		result.entities[1] == EntityInText(EntityType::Bold, 0, kReplyQuoteLimit),
		"formatting clipped to the cut");
}

TEST_CASE("ReplyQuoteSkipsEmptyQuotes") {
	const auto text = TextWithEntities{ u"answer"_q };
	Require(
		QuoteDeletedMessage({ u" \n "_q }, text).text == text.text,
		"blank quote leaves the answer alone");
	const auto padding = QString(kReplyQuoteLimit - 1, QChar('b'));
	const auto emoji = QString(QChar(0xD83D)) + QChar(0xDE00);
	const auto cut = QuoteDeletedMessage({ padding + emoji }, {});
	Require(
		cut.text == padding + QChar(0x2026),
		"surrogate pairs are never split");
}
