#include "serein/features/keyword_alerts/model/keywords.h"

#include "serein/core/patterns.h"
#include "base/basic_types.h"
#include "serein/schema/gen/settings/filters.h"

#include <QtCore/QStringList>

#include <algorithm>
#include <set>
#include <tuple>

namespace Serein::Notifications {
namespace {

constexpr auto kMaxText = 16384;

[[nodiscard]] std::optional<KeywordRule> ParseExpression(const QString &line) {
	if (line.size() < 3 || !line.startsWith(u'/')) {
		return std::nullopt;
	}
	const auto ignoreCase = line.endsWith(u"/i"_q);
	if (!ignoreCase && !line.endsWith(u'/')) {
		return std::nullopt;
	}
	const auto pattern = line.mid(1, line.size() - (ignoreCase ? 3 : 2));
	if (pattern.isEmpty()) {
		return std::nullopt;
	}
	return KeywordRule{
		.pattern = pattern,
		.regex = true,
		.caseSensitive = !ignoreCase,
	};
}

[[nodiscard]] auto Key(const KeywordRule &rule) {
	return std::tuple(rule.pattern, rule.regex, rule.caseSensitive);
}

} // namespace

bool ValidKeywordRule(const KeywordRule &rule) {
	const auto length = rule.pattern.toUcs4().size();
	return length <= kMaxKeywordLength
		&& !rule.pattern.trimmed().isEmpty()
		&& !rule.pattern.contains(u'\n')
		&& (!rule.regex || SafePattern(rule.pattern, !rule.caseSensitive).isValid());
}

bool ValidKeywordAlerts(const KeywordAlerts &value) {
	auto seen = std::set<std::tuple<QString, bool, bool>>();
	return std::ranges::all_of(value.rules, [&](const KeywordRule &rule) {
		return ValidKeywordRule(rule) && seen.insert(Key(rule)).second;
	});
}

std::optional<KeywordAlerts> ReadKeywordAlerts(const QByteArray &raw) {
	return raw.isEmpty()
		? std::make_optional(KeywordAlerts())
		: ParseKeywordAlerts(raw);
}

std::vector<KeywordRule> ParseKeywordLines(const QString &text) {
	auto result = std::vector<KeywordRule>();
	auto seen = std::set<std::tuple<QString, bool, bool>>();
	for (const auto &raw : text.split(u'\n')) {
		const auto line = raw.trimmed();
		if (line.isEmpty()) {
			continue;
		}
		auto rule = ParseExpression(line).value_or(KeywordRule{
			.pattern = line,
		});
		if (seen.insert(Key(rule)).second) {
			result.push_back(std::move(rule));
		}
	}
	return result;
}

QString FormatKeywordLines(const std::vector<KeywordRule> &rules) {
	auto lines = QStringList();
	for (const auto &rule : rules) {
		lines.push_back(!rule.regex
			? rule.pattern
			: (u'/' + rule.pattern + (rule.caseSensitive ? u"/"_q : u"/i"_q)));
	}
	return lines.join(u'\n');
}

bool Matches(const KeywordAlerts &config, const QString &text, bool channel) {
	if (!config.enabled
		|| text.isEmpty()
		|| (channel && !config.includeChannels)) {
		return false;
	}
	const auto view = QStringView(text).left(kMaxText);
	return std::ranges::any_of(config.rules, [&](const KeywordRule &rule) {
		const auto sensitivity = rule.caseSensitive
			? Qt::CaseSensitive
			: Qt::CaseInsensitive;
		return rule.regex
			? CachedSafePattern(rule.pattern, !rule.caseSensitive)
				.matchView(view).hasMatch()
			: view.contains(rule.pattern, sensitivity);
	});
}

} // namespace Serein::Notifications

namespace Serein::Filters {

bool ValidKeywordAlertsBytes(const QByteArray &value) {
	return Notifications::ReadKeywordAlerts(value).has_value();
}

} // namespace Serein::Filters
