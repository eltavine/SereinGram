#pragma once

#include "base/assertion.h"
#include "ui/text/text_entity.h"

namespace Serein::HistoryFeature {

inline constexpr auto kReplyQuoteLimit = 1024;

// Other clients cannot resolve a reply to a message kept only on this device.
[[nodiscard]] TextWithEntities QuoteDeletedMessage(
	const TextWithEntities &quote,
	const TextWithEntities &text);

} // namespace Serein::HistoryFeature
