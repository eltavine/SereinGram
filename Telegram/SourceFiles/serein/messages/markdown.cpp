#include "serein/messages/markdown.h"

#include "base/basic_types.h"

#include <algorithm>
#include <vector>

namespace Serein::Messages {
namespace {

constexpr auto kMaxLanguageLength = 32;

enum class Kind {
	Plain,
	Raw,
	Style,
	Code,
	Quote,
	Pre,
};

struct Span {
	EntityType type = EntityType::Invalid;
	int from = 0;
	int till = 0;
	QString data;
};

struct Marks {
	QString open;
	QString close;
};

[[nodiscard]] Kind KindOf(EntityType type) {
	switch (type) {
	case EntityType::Url:
	case EntityType::Email:
	case EntityType::Hashtag:
	case EntityType::Cashtag:
	case EntityType::Mention:
	case EntityType::BotCommand:
	case EntityType::Phone:
	case EntityType::BankCard:
	case EntityType::CustomEmoji:
	case EntityType::MediaTimestamp:
		return Kind::Raw;
	case EntityType::Bold:
	case EntityType::Semibold:
	case EntityType::Italic:
	case EntityType::Underline:
	case EntityType::StrikeOut:
	case EntityType::Spoiler:
	case EntityType::Subscript:
	case EntityType::Superscript:
	case EntityType::CustomUrl:
	case EntityType::MentionName:
		return Kind::Style;
	case EntityType::Code: return Kind::Code;
	case EntityType::Blockquote: return Kind::Quote;
	case EntityType::Pre: return Kind::Pre;
	default: return Kind::Plain;
	}
}

[[nodiscard]] int LongestRun(QStringView text, QChar ch) {
	auto result = 0;
	auto current = 0;
	for (const auto c : text) {
		current = (c == ch) ? (current + 1) : 0;
		result = std::max(result, current);
	}
	return result;
}

[[nodiscard]] QString Escape(QStringView text) {
	const auto special = u"\\*_~`[]<>|"_q;
	auto result = QString();
	result.reserve(text.size());
	for (const auto ch : text) {
		if (special.contains(ch)) {
			result.append(u'\\');
		}
		result.append(ch);
	}
	return result;
}

[[nodiscard]] QString LinkTarget(const Span &span) {
	if (span.type != EntityType::MentionName) {
		return span.data;
	}
	auto ok = false;
	const auto id = span.data.section(u'.', 0, 0).toULongLong(&ok);
	return (ok && id) ? (u"tg://user?id="_q + QString::number(id)) : QString();
}

[[nodiscard]] QString Destination(QString url) {
	const auto wrap = std::any_of(url.begin(), url.end(), [](QChar ch) {
		return ch.isSpace() || ch == u'(' || ch == u')';
	});
	if (!wrap) {
		return url;
	}
	url.replace(u'<', u"%3C"_q).replace(u'>', u"%3E"_q);
	return u'<' + url + u'>';
}

[[nodiscard]] Marks MarksFor(const Span &span, QStringView text) {
	switch (span.type) {
	case EntityType::Bold:
	case EntityType::Semibold: return { u"**"_q, u"**"_q };
	case EntityType::Italic: {
		const auto intraword = (span.from > 0
				&& text[span.from - 1].isLetterOrNumber())
			|| (span.till < text.size() && text[span.till].isLetterOrNumber());
		const auto mark = intraword ? u"*"_q : u"_"_q;
		return { mark, mark };
	}
	case EntityType::Underline: return { u"<u>"_q, u"</u>"_q };
	case EntityType::StrikeOut: return { u"~~"_q, u"~~"_q };
	case EntityType::Spoiler: return { u"||"_q, u"||"_q };
	case EntityType::Subscript: return { u"<sub>"_q, u"</sub>"_q };
	case EntityType::Superscript: return { u"<sup>"_q, u"</sup>"_q };
	case EntityType::CustomUrl:
	case EntityType::MentionName: {
		const auto target = LinkTarget(span);
		return target.isEmpty()
			? Marks()
			: Marks{ u"["_q, u"]("_q + Destination(target) + u')' };
	}
	case EntityType::Code: {
		const auto content = text.mid(span.from, span.till - span.from);
		const auto fence = QString(LongestRun(content, u'`') + 1, u'`');
		const auto pad = (content.startsWith(u'`') || content.endsWith(u'`'))
			? u" "_q
			: QString();
		return { fence + pad, pad + fence };
	}
	default: return {};
	}
}

class InlineWriter final {
public:
	InlineWriter(QStringView text, std::vector<Span> spans)
	: _text(text)
	, _spans(std::move(spans)) {
		_marks.reserve(_spans.size());
		for (const auto &span : _spans) {
			_marks.push_back(MarksFor(span, _text));
		}
	}

	[[nodiscard]] QString write(int from, int till) {
		auto boundaries = std::vector<int>{ from, till };
		for (const auto &span : _spans) {
			boundaries.push_back(span.from);
			boundaries.push_back(span.till);
		}
		std::ranges::sort(boundaries);
		const auto [first, last] = std::ranges::unique(boundaries);
		boundaries.erase(first, last);
		for (auto i = std::size_t(); i + 1 < boundaries.size(); ++i) {
			segment(boundaries[i], boundaries[i + 1]);
		}
		closeTo(0);
		return std::move(_result);
	}

private:
	[[nodiscard]] bool covers(std::size_t index, int from, int till) const {
		return _spans[index].from <= from && _spans[index].till >= till;
	}

	void segment(int from, int till) {
		auto active = std::vector<std::size_t>();
		auto verbatim = false;
		for (auto i = std::size_t(); i != _spans.size(); ++i) {
			if (!covers(i, from, till)) {
				continue;
			}
			const auto kind = KindOf(_spans[i].type);
			verbatim = verbatim || (kind == Kind::Raw) || (kind == Kind::Code);
			if (kind != Kind::Raw && !_marks[i].open.isEmpty()) {
				active.push_back(i);
			}
		}
		auto keep = std::size_t();
		while (keep < _open.size()
			&& std::ranges::find(active, _open[keep]) != active.end()) {
			++keep;
		}
		closeTo(keep);
		auto opening = std::vector<std::size_t>();
		for (const auto index : active) {
			if (std::ranges::find(_open, index) == _open.end()) {
				opening.push_back(index);
			}
		}
		std::ranges::sort(opening, [&](std::size_t a, std::size_t b) {
			const auto codeA = (_spans[a].type == EntityType::Code);
			const auto codeB = (_spans[b].type == EntityType::Code);
			if (codeA != codeB) {
				return codeB;
			}
			return (_spans[a].from != _spans[b].from)
				? (_spans[a].from < _spans[b].from)
				: (_spans[a].till > _spans[b].till);
		});
		auto content = _text.mid(from, till - from);
		if (!opening.empty() && !hasCode(opening)) {
			auto leading = 0;
			while (leading < content.size() && content[leading].isSpace()) {
				++leading;
			}
			_result += content.left(leading);
			content = content.mid(leading);
		}
		for (const auto index : opening) {
			_result += _marks[index].open;
			_open.push_back(index);
		}
		_result += verbatim ? content.toString() : Escape(content);
	}

	[[nodiscard]] bool hasCode(const std::vector<std::size_t> &list) const {
		return std::ranges::any_of(list, [&](std::size_t index) {
			return _spans[index].type == EntityType::Code;
		});
	}

	void closeTo(std::size_t keep) {
		if (_open.size() <= keep) {
			return;
		}
		const auto closing = std::vector<std::size_t>(
			_open.begin() + keep,
			_open.end());
		auto trailing = 0;
		if (!hasCode(closing)) {
			while (trailing < _result.size()
				&& _result[_result.size() - 1 - trailing].isSpace()) {
				++trailing;
			}
		}
		const auto tail = _result.right(trailing);
		_result.chop(trailing);
		while (_open.size() > keep) {
			_result += _marks[_open.back()].close;
			_open.pop_back();
		}
		_result += tail;
	}

	QStringView _text;
	std::vector<Span> _spans;
	std::vector<Marks> _marks;
	std::vector<std::size_t> _open;
	QString _result;

};

[[nodiscard]] QString Language(const QString &data) {
	auto result = QString();
	for (const auto ch : data) {
		const auto allowed = ch.isLetterOrNumber()
			|| ch == u'_' || ch == u'+' || ch == u'#'
			|| ch == u'.' || ch == u'-';
		if (!allowed || result.size() == kMaxLanguageLength) {
			break;
		}
		result.append(ch);
	}
	return result;
}

[[nodiscard]] QString Fenced(QStringView content, const QString &language) {
	const auto fence = QString(std::max(3, LongestRun(content, u'`') + 1), u'`');
	return fence + Language(language) + u'\n'
		+ content.toString()
		+ (content.endsWith(u'\n') ? QString() : u"\n"_q)
		+ fence;
}

[[nodiscard]] QString Quoted(QString inner) {
	while (inner.endsWith(u'\n')) {
		inner.chop(1);
	}
	auto lines = inner.split(u'\n');
	for (auto &line : lines) {
		line = line.isEmpty() ? u">"_q : (u"> "_q + line);
	}
	return lines.join(u'\n');
}

} // namespace

QString ToMarkdown(const TextWithEntities &text) {
	const auto size = int(text.text.size());
	auto spans = std::vector<Span>();
	for (const auto &entity : text.entities) {
		const auto from = std::clamp(entity.offset(), 0, size);
		const auto till = std::clamp(entity.offset() + entity.length(), from, size);
		if (till > from && KindOf(entity.type()) != Kind::Plain) {
			spans.push_back({ entity.type(), from, till, entity.data() });
		}
	}
	std::ranges::stable_sort(spans, {}, &Span::from);
	auto blocks = std::vector<Span>();
	for (const auto &span : spans) {
		const auto kind = KindOf(span.type);
		if ((kind == Kind::Quote || kind == Kind::Pre)
			&& (blocks.empty() || span.from >= blocks.back().till)) {
			blocks.push_back(span);
		}
	}
	const auto inlineRange = [&](int from, int till) {
		auto clipped = std::vector<Span>();
		for (const auto &span : spans) {
			const auto kind = KindOf(span.type);
			const auto inside = std::max(span.from, from) < std::min(span.till, till);
			if (inside && (kind == Kind::Raw || kind == Kind::Style || kind == Kind::Code)) {
				auto copy = span;
				copy.from = std::max(span.from, from);
				copy.till = std::min(span.till, till);
				clipped.push_back(std::move(copy));
			}
		}
		return InlineWriter(text.text, std::move(clipped)).write(from, till);
	};
	auto result = QString();
	auto previous = Kind::Plain;
	const auto separate = [&] {
		const auto wanted = (previous == Kind::Quote) ? 2 : 1;
		auto have = 0;
		while (have < result.size() && result[result.size() - 1 - have] == u'\n') {
			++have;
		}
		if (!result.isEmpty() && have < wanted) {
			result += QString(wanted - have, u'\n');
		}
	};
	const auto appendText = [&](int from, int till) {
		auto chunk = inlineRange(from, till);
		if (previous != Kind::Plain) {
			auto skip = 0;
			while (skip < chunk.size() && chunk[skip] == u'\n') {
				++skip;
			}
			chunk.remove(0, skip);
			if (chunk.isEmpty()) {
				return;
			}
			separate();
		}
		result += chunk;
		previous = Kind::Plain;
	};
	auto position = 0;
	for (const auto &block : blocks) {
		appendText(position, block.from);
		separate();
		const auto content = QStringView(text.text).mid(
			block.from,
			block.till - block.from);
		result += (block.type == EntityType::Pre)
			? Fenced(content, block.data)
			: Quoted(inlineRange(block.from, block.till));
		previous = KindOf(block.type);
		position = block.till;
	}
	appendText(position, size);
	return result;
}

} // namespace Serein::Messages
