#include "serein/features/history/model/reply_quote.h"

#include "base/basic_types.h"

#include <algorithm>

namespace Serein::HistoryFeature {
namespace {

constexpr auto kEllipsis = char16_t(0x2026);

[[nodiscard]] bool KeptInQuote(EntityType type) {
	switch (type) {
	case EntityType::Pre:
	case EntityType::Blockquote:
	case EntityType::CustomEmoji:
	case EntityType::Mention:
	case EntityType::MentionName:
		return false;
	default:
		return true;
	}
}

[[nodiscard]] bool Clippable(EntityType type) {
	switch (type) {
	case EntityType::Bold:
	case EntityType::Italic:
	case EntityType::Underline:
	case EntityType::StrikeOut:
	case EntityType::Code:
	case EntityType::Spoiler:
		return true;
	default:
		return false;
	}
}

} // namespace

TextWithEntities QuoteDeletedMessage(
		const TextWithEntities &quote,
		const TextWithEntities &text) {
	const auto &source = quote.text;
	const auto size = int(source.size());
	auto till = std::min(size, kReplyQuoteLimit);
	if (till < size && till > 0 && source[till - 1].isHighSurrogate()) {
		--till;
	}
	const auto cut = (till < size);
	while (till > 0 && source[till - 1].isSpace()) {
		--till;
	}
	auto from = 0;
	while (from < till && source[from].isSpace()) {
		++from;
	}
	if (from == till) {
		return text;
	}
	auto result = TextWithEntities();
	result.text = source.mid(from, till - from);
	if (cut) {
		result.text.append(QChar(kEllipsis));
	}
	result.entities.push_back(EntityInText(
		EntityType::Blockquote,
		0,
		int(result.text.size()),
		u"1"_q));
	for (const auto &entity : quote.entities) {
		const auto type = entity.type();
		const auto start = std::max(entity.offset(), from);
		const auto end = std::min(entity.offset() + entity.length(), till);
		const auto whole = (start == entity.offset())
			&& (end == entity.offset() + entity.length());
		if (KeptInQuote(type) && start < end && (whole || Clippable(type))) {
			result.entities.push_back(EntityInText(
				type,
				start - from,
				end - start,
				entity.data()));
		}
	}
	if (text.text.isEmpty()) {
		return result;
	}
	result.text.append(QChar('\n'));
	const auto shift = int(result.text.size());
	result.text.append(text.text);
	for (auto entity : text.entities) {
		entity.shiftRight(shift);
		result.entities.push_back(entity);
	}
	return result;
}

} // namespace Serein::HistoryFeature
