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
		const DeletedQuote &quote,
		const TextWithEntities &text,
		int limit) {
	const auto &source = quote.text.text;
	auto from = 0;
	auto till = int(source.size());
	while (from < till && source[from].isSpace()) {
		++from;
	}
	while (till > from && source[till - 1].isSpace()) {
		--till;
	}
	const auto header = quote.author.isEmpty()
		? 0
		: (int(quote.author.size()) + 1);
	const auto tail = text.text.isEmpty() ? 0 : (int(text.text.size()) + 1);
	const auto room = limit - header - tail;
	const auto cut = (till - from > std::min(room, kReplyQuoteLimit));
	if (cut) {
		till = from + std::min(room - 1, kReplyQuoteLimit);
		if (till > from && source[till - 1].isHighSurrogate()) {
			--till;
		}
		while (till > from && source[till - 1].isSpace()) {
			--till;
		}
	}
	if (till <= from) {
		return text;
	}
	auto result = TextWithEntities();
	if (header) {
		result.text = quote.author + QChar('\n');
	}
	result.text.append(source.mid(from, till - from));
	if (cut) {
		result.text.append(QChar(kEllipsis));
	}
	result.entities.push_back(EntityInText(
		EntityType::Blockquote,
		0,
		int(result.text.size()),
		u"1"_q));
	if (header) {
		result.entities.push_back(EntityInText(
			EntityType::Bold,
			0,
			int(quote.author.size())));
	}
	for (const auto &entity : quote.text.entities) {
		const auto type = entity.type();
		const auto start = std::max(entity.offset(), from);
		const auto end = std::min(entity.offset() + entity.length(), till);
		const auto whole = (start == entity.offset())
			&& (end == entity.offset() + entity.length());
		if (KeptInQuote(type) && start < end && (whole || Clippable(type))) {
			result.entities.push_back(EntityInText(
				type,
				start - from + header,
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
