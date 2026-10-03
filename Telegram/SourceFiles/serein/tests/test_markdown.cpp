#include "serein/messages/markdown.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

namespace {

[[nodiscard]] QString Markdown(
		const QString &text,
		const EntitiesInText &entities = {}) {
	return Serein::Messages::ToMarkdown({ text, entities });
}

[[nodiscard]] EntityInText Entity(
		EntityType type,
		int offset,
		int length,
		const QString &data = QString()) {
	return EntityInText(type, offset, length, data);
}

} // namespace

TEST_CASE("Markdown") {
	using Type = EntityType;
	Require(Markdown(u"a*b_c [x] <y> |z| \\ ~`"_q)
		== u"a\\*b\\_c \\[x\\] \\<y\\> \\|z\\| \\\\ \\~\\`"_q,
		"plain text not escaped");
	Require(Markdown(u"Hello world"_q, { Entity(Type::Bold, 0, 5) })
		== u"**Hello** world"_q, "bold");
	Require(Markdown(u"bold text"_q, { Entity(Type::Bold, 0, 5) })
		== u"**bold** text"_q, "space kept inside the marks");
	Require(Markdown(u"say hi now"_q, { Entity(Type::Italic, 4, 2) })
		== u"say _hi_ now"_q, "italic word");
	Require(Markdown(u"unbelievable"_q, { Entity(Type::Italic, 2, 6) })
		== u"un*believ*able"_q, "italic inside a word");
	Require(Markdown(u"bold italic"_q, {
			Entity(Type::Bold, 0, 11),
			Entity(Type::Italic, 5, 6) })
		== u"**bold _italic_**"_q, "nested styles");
	Require(Markdown(u"abcdef"_q, {
			Entity(Type::Bold, 0, 4),
			Entity(Type::Italic, 2, 4) })
		== u"**ab*cd****ef*"_q, "overlapping styles stay balanced");
	Require(Markdown(u"a`b"_q, { Entity(Type::Code, 0, 3) })
		== u"``a`b``"_q, "code fence longer than its backticks");
	Require(Markdown(u"`x"_q, { Entity(Type::Code, 0, 2) })
		== u"`` `x ``"_q, "code starting with a backtick");
	Require(Markdown(u"*x*"_q, {
			Entity(Type::Bold, 0, 3),
			Entity(Type::Code, 0, 3) })
		== u"**`*x*`**"_q, "code is verbatim inside styles");
	Require(Markdown(u"print(1)"_q, { Entity(Type::Pre, 0, 8, u"python"_q) })
		== u"```python\nprint(1)\n```"_q, "code block");
	Require(Markdown(u"see:\ncode\nend"_q, { Entity(Type::Pre, 5, 4) })
		== u"see:\n```\ncode\n```\nend"_q, "code block between text");
	Require(Markdown(u"x"_q, { Entity(Type::Pre, 0, 1, u"c++ `bad`"_q) })
		== u"```c++\nx\n```"_q, "unsafe code language kept");
	Require(Markdown(u"quote line\nsecond"_q, { Entity(Type::Blockquote, 0, 17) })
		== u"> quote line\n> second"_q, "quote");
	Require(Markdown(u"q\nafter"_q, { Entity(Type::Blockquote, 0, 1) })
		== u"> q\n\nafter"_q, "text after a quote leaves it");
	Require(Markdown(u"before\nq"_q, { Entity(Type::Blockquote, 7, 1) })
		== u"before\n> q"_q, "quote after text");
	Require(Markdown(u"site"_q, {
			Entity(Type::CustomUrl, 0, 4, u"https://e.com/a b"_q) })
		== u"[site](<https://e.com/a b>)"_q, "link with a space");
	Require(Markdown(u"[x]"_q, {
			Entity(Type::CustomUrl, 0, 3, u"https://e.com"_q) })
		== u"[\\[x\\]](https://e.com)"_q, "link text escaped");
	Require(Markdown(u"Bob"_q, { Entity(Type::MentionName, 0, 3, u"123.456"_q) })
		== u"[Bob](tg://user?id=123)"_q, "mention");
	Require(Markdown(u"Bob"_q, { Entity(Type::MentionName, 0, 3, u"x"_q) })
		== u"Bob"_q, "broken mention kept as text");
	Require(Markdown(u"https://a.com/x_y"_q, { Entity(Type::Url, 0, 17) })
		== u"https://a.com/x_y"_q, "url not escaped");
	Require(Markdown(u"secret"_q, { Entity(Type::Spoiler, 0, 6) })
		== u"||secret||"_q, "spoiler");
	Require(Markdown(u"u"_q, { Entity(Type::Underline, 0, 1) })
		== u"<u>u</u>"_q, "underline");
	Require(Markdown(u"\U0001F600 bold"_q, { Entity(Type::Bold, 3, 4) })
		== u"\U0001F600 **bold**"_q, "offsets count UTF-16 units");
	Require(Markdown(u"abc"_q, { Entity(Type::Bold, 5, 100) }) == u"abc"_q,
		"entity outside the text");
}
