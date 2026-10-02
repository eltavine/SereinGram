#include "serein/compose/link_inline_bots.h"

#include "base/basic_types.h"

#include <QtCore/QStringList>

#include <algorithm>

namespace Serein::Compose {
namespace {

constexpr auto kArrow = QLatin1String("=>");

[[nodiscard]] QRegularExpression Compile(const QString &pattern) {
	return QRegularExpression(
		pattern,
		QRegularExpression::CaseInsensitiveOption
			| QRegularExpression::UseUnicodePropertiesOption);
}

} // namespace

bool ValidLinkInlineBots(const LinkInlineBots &value) {
	return std::all_of(value.rules.begin(), value.rules.end(), [](
			const LinkInlineBot &rule) {
		return Compile(rule.pattern).isValid();
	});
}

bool ValidLinkInlineBotsBytes(const QByteArray &value) {
	return ReadLinkInlineBots(value).has_value();
}

std::optional<LinkInlineBots> ReadLinkInlineBots(const QByteArray &raw) {
	return raw.isEmpty()
		? std::make_optional(LinkInlineBots())
		: ParseLinkInlineBots(raw);
}

QByteArray WriteLinkInlineBots(const LinkInlineBots &value) {
	return value.rules.empty()
		? QByteArray()
		: SerializeLinkInlineBots(value);
}

QString FormatLinkInlineBotLines(const LinkInlineBots &value) {
	auto lines = QStringList();
	for (const auto &rule : value.rules) {
		lines.push_back(u'@' + rule.bot + u' ' + kArrow + u' ' + rule.pattern);
	}
	return lines.join(u'\n');
}

std::optional<LinkInlineBots> ParseLinkInlineBotLines(const QString &text) {
	auto result = LinkInlineBots();
	for (const auto &line : text.split(u'\n')) {
		if (line.trimmed().isEmpty()) {
			continue;
		}
		const auto arrow = line.indexOf(kArrow);
		if (arrow < 0) {
			return std::nullopt;
		}
		auto bot = line.left(arrow).trimmed();
		if (bot.startsWith(u'@')) {
			bot = bot.mid(1);
		}
		result.rules.push_back({
			.bot = bot,
			.pattern = line.mid(arrow + kArrow.size()).trimmed(),
		});
	}
	auto error = Codec::Error();
	if (!Validate(result, error, QString()) || !ValidLinkInlineBots(result)) {
		return std::nullopt;
	}
	return result;
}

LinkInlineBotMatcher::LinkInlineBotMatcher(const LinkInlineBots &rules) {
	for (const auto &rule : rules.rules) {
		auto compiled = Compile(rule.pattern);
		if (compiled.isValid()) {
			_rules.emplace_back(rule.bot, std::move(compiled));
		}
	}
}

QString LinkInlineBotMatcher::match(const QString &text) const {
	const auto link = text.trimmed();
	const auto spaced = std::any_of(link.begin(), link.end(), [](QChar ch) {
		return ch.isSpace();
	});
	if (_rules.empty()
		|| link.isEmpty()
		|| link.size() > kMaxInlineBotQuery
		|| link.startsWith(u'@')
		|| spaced) {
		return QString();
	}
	for (const auto &[bot, expression] : _rules) {
		if (expression.match(link).hasMatch()) {
			return bot;
		}
	}
	return QString();
}

} // namespace Serein::Compose
