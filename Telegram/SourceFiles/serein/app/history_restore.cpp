#include "serein/hooks/history.h"

#include "serein/app/history_entities.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/hooks/gen/history.h"
#include "serein/ports/history_store.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "main/main_session.h"

#include <map>
#include <set>

namespace Serein::Hooks {
namespace {

constexpr auto kRestoreLimit = 500;

using RestoredKey = std::pair<PeerId, qint64>;

[[nodiscard]] std::set<RestoredKey> &Restored(
		gsl::not_null<Main::Session*> session) {
	static auto result = std::map<Main::Session*, std::set<RestoredKey>>();
	const auto [i, fresh] = result.try_emplace(session);
	if (fresh) {
		session->lifetime().add([=] { result.erase(session); });
	}
	return i->second;
}

[[nodiscard]] TextWithEntities RestoredText(
		const Serein::History::Record &record) {
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

void Restore(
		gsl::not_null<::History*> history,
		const Serein::History::Record &record) {
	const auto session = &history->session();
	const auto peer = history->peer;
	const auto from = record.fromPeerId
		? PeerId(PeerIdHelper(BareId(record.fromPeerId)))
		: peer->id;
	auto flags = MessageFlag::Local;
	if (peer->isBroadcast()) {
		flags |= MessageFlag::Post;
	} else if (from != peer->id) {
		flags |= MessageFlag::HasFromId;
	}
	if (from == session->userPeerId()) {
		flags |= MessageFlag::Outgoing;
	}
	auto fields = HistoryItemCommonFields{
		.id = session->data().nextLocalMessageId(),
		.flags = flags,
		.from = from,
		.date = TimeId(record.date),
	};
	if (record.topicRootId) {
		fields.replyTo.topicRootId = MsgId(record.topicRootId);
	}
	const auto item = history->makeMessage(
		std::move(fields),
		RestoredText(record),
		MTP_messageMediaEmpty());
	history->insertRestoredMessage(item);
	HistoryFeature::MarkDeletedInPlace(item);
}

void RestoreLoaded(gsl::not_null<::History*> history) {
	const auto session = &history->session();
	const auto store = HistoryStoreFor(session);
	if (!store) {
		return;
	}
	auto minId = MsgId();
	auto maxId = MsgId();
	for (const auto &block : history->blocks) {
		for (const auto &view : block->messages) {
			const auto item = view->data();
			if (!item->isRegular()) {
				continue;
			} else if (!minId || item->id < minId) {
				minId = item->id;
			}
			maxId = std::max(maxId, item->id);
		}
	}
	if (!minId) {
		return;
	}
	const auto peerId = history->peer->id;
	auto &restored = Restored(session);
	const auto records = store->deleted({
		.peerId = qint64(peerId.value),
		.minMessageId = minId.bare,
		.maxMessageId = maxId.bare,
		.limit = kRestoreLimit,
	});
	for (const auto &record : records) {
		const auto key = RestoredKey(peerId, record.messageId);
		if (restored.contains(key)
			|| history->owner().message(peerId, MsgId(record.messageId))) {
			continue;
		}
		restored.insert(key);
		Restore(history, record);
	}
}

} // namespace

void OnHistorySliceAdded(gsl::not_null<::History*> history) {
	const auto session = &history->session();
	if (!HistorySettings::HistoryKeepDeletedInPlace(session)) {
		return;
	}
	const auto peerId = history->peer->id;
	crl::on_main(session, [=] {
		if (const auto loaded = session->data().historyLoaded(peerId)) {
			RestoreLoaded(loaded);
		}
	});
}

} // namespace Serein::Hooks
