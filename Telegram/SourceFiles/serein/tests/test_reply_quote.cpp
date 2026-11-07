#include "serein/features/history/model/reply_quote.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

namespace {

using Serein::HistoryFeature::DeletedQuote;
using Serein::HistoryFeature::kReplyQuoteLimit;
using Serein::HistoryFeature::QuoteDeletedMessage;

constexpr auto kMessageLimit = 4096;

[[nodiscard]] EntityInText Quote(int offset, int length) {
	return EntityInText(EntityType::Blockquote, offset, length, u"1"_q);
}

[[nodiscard]] DeletedQuote Anonymous(TextWithEntities text) {
	return { .text = std::move(text) };
}

} // namespace

TEST_CASE("ReplyQuotePrependsCollapsedQuote") {
	const auto result = QuoteDeletedMessage(
		Anonymous({ u"  deleted words\n\n"_q }),
		{ u"my answer"_q, { EntityInText(EntityType::Bold, 3, 6) } },
		kMessageLimit);
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
	const auto result = QuoteDeletedMessage(
		Anonymous(quote),
		{},
		kMessageLimit);
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
	const auto result = QuoteDeletedMessage(
		Anonymous(quote),
		{ u"ok"_q },
		kMessageLimit);
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
		QuoteDeletedMessage(
			Anonymous({ u" \n "_q }),
			text,
			kMessageLimit).text == text.text,
		"blank quote leaves the answer alone");
	const auto padding = QString(kReplyQuoteLimit - 1, QChar('b'));
	const auto emoji = QString(QChar(0xD83D)) + QChar(0xDE00);
	const auto cut = QuoteDeletedMessage(
		Anonymous({ padding + emoji }),
		{},
		kMessageLimit);
	Require(
		cut.text == padding + QChar(0x2026),
		"surrogate pairs are never split");
}

TEST_CASE("ReplyQuoteNamesTheAuthor") {
	const auto result = QuoteDeletedMessage(
		{
			.author = u"Alice"_q,
			.text = { u"hi there"_q, { EntityInText(EntityType::Italic, 3, 5) } },
		},
		{ u"ok"_q },
		kMessageLimit);
	Require(
		result.text == u"Alice\nhi there\nok"_q,
		"author line opens the quote");
	Require(result.entities.size() == 3, "quote, author and italic");
	Require(result.entities[0] == Quote(0, 14), "quote covers the author");
	Require(
		result.entities[1] == EntityInText(EntityType::Bold, 0, 5),
		"author in bold");
	Require(
		result.entities[2] == EntityInText(EntityType::Italic, 9, 5),
		"quoted formatting moves behind the author");
}

TEST_CASE("ReplyQuoteFitsInOneMessage") {
	const auto limit = 20;
	const auto result = QuoteDeletedMessage(
		Anonymous({ QString(30, QChar('a')) }),
		{ u"answer"_q },
		limit);
	Require(int(result.text.size()) == limit, "quote shrinks to the room left");
	Require(
		result.text == QString(12, QChar('a')) + QChar(0x2026) + u"\nanswer"_q,
		"cut quote keeps the whole answer");
	const auto named = QuoteDeletedMessage(
		{ .author = u"Alice"_q, .text = { u"hello"_q } },
		{ u"answer"_q },
		12);
	Require(named.text == u"answer"_q, "no room for the author and a word");
	const auto full = QString(limit, QChar('c'));
	Require(
		QuoteDeletedMessage(Anonymous({ u"x"_q }), { full }, limit).text == full,
		"a full message gets no quote");
}
