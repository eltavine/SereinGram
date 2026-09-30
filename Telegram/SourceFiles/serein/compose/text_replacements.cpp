#include "serein/compose/text_replacements.h"

#include "serein/compose/options.h"
#include "base/basic_types.h"

#include <QtCore/QStringList>

#include <algorithm>
#include <set>

namespace Serein::Compose {
namespace {

constexpr auto kArrow = QLatin1String("=>");

} // namespace

bool ValidTextReplacements(const TextReplacements &value) {
	auto seen = std::set<QString>();
	return std::all_of(value.rules.begin(), value.rules.end(), [&](
			const TextReplacement &rule) {
		return !rule.from.contains(u'\n')
			&& !rule.to.contains(u'\n')
			&& seen.insert(rule.from).second;
	});
}

bool ValidTextReplacementsBytes(const QByteArray &value) {
	return ReadTextReplacements(value).has_value();
}

std::optional<TextReplacements> ReadTextReplacements(const QByteArray &raw) {
	return raw.isEmpty()
		? std::make_optional(TextReplacements())
		: ParseTextReplacements(raw);
}

QByteArray WriteTextReplacements(const TextReplacements &value) {
	return value.rules.empty()
		? QByteArray()
		: SerializeTextReplacements(value);
}

QString FormatReplacementLines(const TextReplacements &value) {
	auto lines = QStringList();
	for (const auto &rule : value.rules) {
		lines.push_back(rule.from + u' ' + kArrow + u' ' + rule.to);
	}
	return lines.join(u'\n');
}

std::optional<TextReplacements> ParseReplacementLines(const QString &text) {
	auto result = TextReplacements();
	for (const auto &line : text.split(u'\n')) {
		if (line.trimmed().isEmpty()) {
			continue;
		}
		const auto arrow = line.indexOf(kArrow);
		if (arrow < 0) {
			return std::nullopt;
		}
		result.rules.push_back({
			.from = line.left(arrow).trimmed(),
			.to = line.mid(arrow + kArrow.size()).trimmed(),
		});
	}
	auto error = Codec::Error();
	if (!Validate(result, error, QString()) || !ValidTextReplacements(result)) {
		return std::nullopt;
	}
	return result;
}

} // namespace Serein::Compose
