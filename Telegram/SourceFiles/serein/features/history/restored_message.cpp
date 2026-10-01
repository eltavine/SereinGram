#include "serein/features/history/restored_message.h"

#include "serein/app/history_entities.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "main/main_session.h"

namespace Serein::HistoryFeature {
namespace {

[[nodiscard]] TextWithEntities RestoredText(const History::Record &record) {
	if (record.text.isEmpty()) {
		return TextWithEntities{ record.mediaSummary };
	}
	auto result = TextWithEntities{ record.text };
	for (const auto &entity : record.entities) {
		if (const auto type = App::EntityTypeFromName(entity.type)) {
			result.entities.push_back(EntityInText(
				*type,
				entity.offset,
				entity.length,
				entity.data));
		}
	}
	return result;
}

} // namespace

not_null<HistoryItem*> MakeRestoredMessage(
		not_null<::History*> history,
		const History::Record &record,
		MsgId id,
		MessageFlags flags) {
	const auto session = &history->session();
	const auto peer = history->peer;
	const auto from = record.fromPeerId
		? PeerId(PeerIdHelper(BareId(record.fromPeerId)))
		: peer->id;
	if (peer->isBroadcast()) {
		flags |= MessageFlag::Post;
	} else if (from != peer->id) {
		flags |= MessageFlag::HasFromId;
	}
	if (from == session->userPeerId()) {
		flags |= MessageFlag::Outgoing;
	}
	auto fields = HistoryItemCommonFields{
		.id = id,
		.flags = flags,
		.from = from,
		.date = TimeId(record.date),
	};
	if (record.topicRootId) {
		fields.replyTo.topicRootId = MsgId(record.topicRootId);
	}
	return history->makeMessage(
		std::move(fields),
		RestoredText(record),
		MTP_messageMediaEmpty());
}

} // namespace Serein::HistoryFeature
