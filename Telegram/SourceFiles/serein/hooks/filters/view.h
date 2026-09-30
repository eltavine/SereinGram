#pragma once

class HistoryItem;
class PeerData;
struct TextWithEntities;

namespace Serein::Hooks::Filters {

[[nodiscard]] bool Hidden(HistoryItem *item);
[[nodiscard]] TextWithEntities DisplayText(
	HistoryItem *item,
	const TextWithEntities &source);
[[nodiscard]] TextWithEntities ReplyText(
	HistoryItem *quoted,
	const TextWithEntities &text);
[[nodiscard]] bool HiddenPeer(PeerData *peer);

} // namespace Serein::Hooks::Filters
