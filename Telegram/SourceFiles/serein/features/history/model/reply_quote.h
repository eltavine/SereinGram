#pragma once

#include "base/assertion.h"
#include "ui/text/text_entity.h"

namespace Serein::HistoryFeature {

inline constexpr auto kReplyQuoteLimit = 1024;

struct DeletedQuote {
	QString author;
	TextWithEntities text;
};

// Other clients cannot resolve a reply to a message kept only on this device.
[[nodiscard]] TextWithEntities QuoteDeletedMessage(
	const DeletedQuote &quote,
	const TextWithEntities &text,
	int limit);

} // namespace Serein::HistoryFeature
