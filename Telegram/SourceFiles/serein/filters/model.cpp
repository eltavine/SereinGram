#include "serein/filters/model.h"

#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>
#include <QtCore/QSet>
#include <QtCore/QUuid>

#include <algorithm>

namespace Serein::Filters {
namespace {

constexpr auto kMaxText = 16384;
constexpr auto kMaxMatches = 256;
constexpr auto kMaxWorkMs = 20;
constexpr auto kMaxConfigBytes = 128 * 1024;

QString RegexPrefix() {
	return u"(*NO_JIT)(*LIMIT_MATCH=10000)(*LIMIT_DEPTH=64)(*LIMIT_HEAP=1024)"_q;
}

QRegularExpression CompilePattern(const QString &pattern, bool caseInsensitive) {
	return QRegularExpression(
		RegexPrefix() + pattern,
		QRegularExpression::UseUnicodePropertiesOption
			| (caseInsensitive
				? QRegularExpression::CaseInsensitiveOption
				: QRegularExpression::NoPatternOption));
}

bool ValidId(const QString &id) {
	auto ok = false;
	const auto number = id.toULongLong(&ok);
	return ok && number && QString::number(number) == id;
}

bool ValidText(const QString &text, int limit, bool allowEmpty) {
	return (allowEmpty || !text.isEmpty()) && text.size() <= limit
		&& !text.contains(QChar(0))
		&& QString::fromUtf8(text.toUtf8()) == text;
}

struct Edit {
	int start = 0;
	int end = 0;
	QString replacement;
};

bool ApplyEdits(TextWithEntities &text, const std::vector<Edit> &edits) {
	const auto original = text;
	auto offsets = std::vector<int>(original.text.size() + 1, -1);
	auto result = TextWithEntities();
	auto cursor = 0;
	for (const auto &edit : edits) {
		if (edit.start < cursor || edit.end <= edit.start
			|| edit.end > original.text.size()) {
			return false;
		}
		for (; cursor < edit.start; ++cursor) {
			offsets[cursor] = result.text.size();
			result.text += original.text[cursor];
		}
		offsets[edit.start] = result.text.size();
		result.text += edit.replacement;
		cursor = edit.end;
		offsets[cursor] = result.text.size();
	}
	for (; cursor < original.text.size(); ++cursor) {
		offsets[cursor] = result.text.size();
		result.text += original.text[cursor];
	}
	offsets.back() = result.text.size();
	if (result.text.size() > kMaxText) {
		return false;
	}
	for (const auto &entity : original.entities) {
		if (!entity.validForText(original.text.size())) {
			return false;
		}
		const auto start = entity.offset();
		const auto end = start + entity.length();
		const auto overlaps = std::any_of(edits.begin(), edits.end(), [&](const Edit &edit) {
			return edit.start < end && edit.end > start;
		});
		if (overlaps || offsets[start] < 0 || offsets[end] < offsets[start]) {
			continue;
		}
		auto adjusted = entity;
		adjusted.shiftLeft(start - offsets[start]);
		adjusted.shrinkFromRight(entity.length()
			- (offsets[end] - offsets[start]));
		result.entities.push_back(std::move(adjusted));
	}
	text = std::move(result);
	return true;
}

std::vector<Edit> ZalgoEdits(const QString &text) {
	auto edits = std::vector<Edit>();
	auto marks = 0;
	for (auto i = 0; i < text.size();) {
		const auto start = i;
		auto scalar = uint(text[i++].unicode());
		if (QChar::isHighSurrogate(scalar) && i < text.size()) {
			scalar = QChar::surrogateToUcs4(QChar(scalar), text[i++]);
		}
		const auto category = QChar::category(scalar);
		const auto combining = category == QChar::Mark_NonSpacing
			|| category == QChar::Mark_SpacingCombining
			|| category == QChar::Mark_Enclosing;
		marks = combining ? marks + 1 : 0;
		if (marks > 3 && scalar != 0xFE0E && scalar != 0xFE0F
			&& scalar != 0x20E3
			&& !(scalar >= 0xE0100 && scalar <= 0xE01EF)) {
			edits.push_back({ start, i, QString() });
		}
	}
	return edits;
}

} // namespace

QJsonObject Defaults() {
	return {
		{ u"version"_q, 1 },
		{ u"enabled"_q, false },
		{ u"filterOutgoing"_q, false },
		{ u"hideBlocked"_q, false },
		{ u"stripZalgo"_q, false },
		{ u"hiddenAuthors"_q, QJsonArray() },
		{ u"excludedPeers"_q, QJsonArray() },
		{ u"rules"_q, QJsonArray() },
	};
}

bool ValidFilterRules(const FilterRules &value) {
	if (SerializeFilterRules(value).size() > kMaxConfigBytes
		|| !std::ranges::all_of(value.hiddenAuthors, ValidId)
		|| !std::ranges::all_of(value.excludedPeers, ValidId)) {
		return false;
	}
	auto seen = QSet<QString>();
	for (const auto &rule : value.rules) {
		if (seen.contains(rule.id)
			|| QUuid(rule.id).isNull()
			|| !ValidText(rule.title, 128, false)
			|| !ValidText(rule.pattern, 2048, false)
			|| !ValidText(rule.replacement, 4096, true)
			|| (rule.reversed && rule.action != u"hide"_q)
			|| !CompilePattern(rule.pattern, rule.caseInsensitive).isValid()) {
			return false;
		}
		seen.insert(rule.id);
	}
	return true;
}

bool Validate(const QByteArray &raw) {
	return raw.isEmpty() || ParseFilterRules(raw).has_value();
}

Result Apply(
		const QByteArray &raw,
		const TextWithEntities &source,
		const QString &author,
		const QString &peer,
		bool blocked,
		bool outgoing,
		const QString &searchable) {
	auto result = Result{ .text = source };
	if (raw.isEmpty()) {
		return result;
	}
	const auto config = ParseFilterRules(raw);
	if (!config) {
		result.error = u"invalid filter configuration"_q;
		return result;
	}
	const auto contains = [](const std::vector<QString> &list, const QString &id) {
		return std::find(list.begin(), list.end(), id) != list.end();
	};
	if (!config->enabled
		|| contains(config->excludedPeers, peer)
		|| (outgoing && !config->filterOutgoing)) {
		return result;
	}
	if ((blocked && config->hideBlocked)
		|| contains(config->hiddenAuthors, author)) {
		result.hidden = true;
		return result;
	}
	if (source.text.size() > kMaxText
		|| (!searchable.isNull() && searchable.size() > kMaxText)) {
		result.error = u"filter text length limit"_q;
		return result;
	}
	auto timer = QElapsedTimer();
	timer.start();
	if (config->stripZalgo
		&& !ApplyEdits(result.text, ZalgoEdits(result.text.text))) {
		result.error = u"filter Zalgo edit failed"_q;
		return { .text = source, .error = result.error };
	}
	for (const auto &rule : config->rules) {
		if (!rule.enabled) {
			continue;
		}
		const auto expression = CompilePattern(
			rule.pattern,
			rule.caseInsensitive);
		const auto &action = rule.action;
		const auto text = action == u"hide" && !searchable.isNull()
			? searchable : result.text.text;
		auto edits = std::vector<Edit>();
		auto matched = false;
		for (auto offset = 0; offset <= text.size();) {
			if (timer.elapsed() >= kMaxWorkMs) {
				result.error = u"filter runtime limit"_q;
				break;
			}
			const auto match = expression.match(text, offset);
			if (!match.isValid()) {
				result.error = u"filter regex work limit"_q;
				break;
			}
			if (!match.hasMatch()) {
				break;
			}
			if (++result.matches > kMaxMatches) {
				result.error = u"filter match count limit"_q;
				break;
			}
			matched = true;
			if (action == u"hide"_q) {
				break;
			}
			const auto start = int(match.capturedStart());
			const auto end = int(match.capturedEnd());
			if (end <= start) {
				offset = end + 1;
				continue;
			}
			edits.push_back({ start, end,
				action == u"mask"_q ? u"•••"_q : rule.replacement });
			offset = end;
		}
		if (!result.error.isEmpty()) {
			break;
		}
		if (action == u"hide"_q) {
			if (matched != rule.reversed) {
				result.hidden = true;
				return result;
			}
		} else if (!ApplyEdits(result.text, edits)) {
			result.error = u"filter replacement limit"_q;
			break;
		}
	}
	if (!result.error.isEmpty()) {
		return { .text = source, .error = result.error,
			.matches = result.matches };
	}
	return result;
}

} // namespace Serein::Filters
