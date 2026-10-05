#include "serein/features/history/restored_message.h"

#include "serein/features/history/entities.h"
#include "serein/features/history/wire.h"
#include "data/data_peer.h"
#include "history/admin_log/history_admin_log_item.h"
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
		if (const auto type = EntityTypeFromName(entity.type)) {
			result.entities.push_back(EntityInText(
				*type,
				entity.offset,
				entity.length,
				entity.data));
		}
	}
	return result;
}

[[nodiscard]] HistoryItem *FromWire(
		not_null<::History*> history,
		const History::Record &record,
		const RestoreArgs &args,
		TimeId date) {
	const auto parsed = ParseMessage(record.tlMessage);
	if (!parsed) {
		return nullptr;
	}
	return parsed->match([&](const MTPDmessage &data) -> HistoryItem* {
		if (peerFromMTP(data.vpeer_id()) != history->peer->id) {
			return nullptr;
		}
		const auto direction = args.asLogEntry
			? MessageFlag::AdminLogEntry
			: ((data.is_out() ? MessageFlag::Outgoing : MessageFlag())
				| (data.is_post() ? MessageFlag::Post : MessageFlag()));
		const auto flags = args.flags | direction;
		return history->createItem(
			args.id,
			AdminLog::PrepareLogMessage(*parsed, date),
			flags).get();
	}, [](const auto &) -> HistoryItem* {
		return nullptr;
	});
}

[[nodiscard]] not_null<HistoryItem*> FromText(
		not_null<::History*> history,
		const History::Record &record,
		const RestoreArgs &args,
		TimeId date) {
	const auto session = &history->session();
	const auto peer = history->peer;
	const auto from = record.fromPeerId
		? PeerId(PeerIdHelper(BareId(record.fromPeerId)))
		: peer->id;
	auto flags = args.flags;
	if (args.asLogEntry) {
		flags |= MessageFlag::AdminLogEntry;
		if (from != peer->id) {
			flags |= MessageFlag::HasFromId;
		}
	} else {
		if (peer->isBroadcast()) {
			flags |= MessageFlag::Post;
		} else if (from != peer->id) {
			flags |= MessageFlag::HasFromId;
		}
		if (from == session->userPeerId()) {
			flags |= MessageFlag::Outgoing;
		}
	}
	auto fields = HistoryItemCommonFields{
		.id = args.id,
		.flags = flags,
		.from = from,
		.date = date,
	};
	if (record.topicRootId) {
		fields.replyTo.topicRootId = MsgId(record.topicRootId);
	}
	return history->makeMessage(
		std::move(fields),
		RestoredText(record),
		MTP_messageMediaEmpty());
}

} // namespace

not_null<HistoryItem*> MakeRestoredMessage(
		not_null<::History*> history,
		const History::Record &record,
		RestoreArgs args) {
	const auto date = args.date ? args.date : TimeId(record.date);
	if (const auto restored = FromWire(history, record, args, date)) {
		return restored;
	}
	return FromText(history, record, args, date);
}

} // namespace Serein::HistoryFeature
