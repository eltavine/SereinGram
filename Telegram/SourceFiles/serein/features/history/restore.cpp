#include "serein/hooks/history.h"

#include "serein/features/history/backend.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/restored_message.h"
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

void Restore(
		gsl::not_null<::History*> history,
		const Serein::History::Record &record) {
	const auto item = HistoryFeature::MakeRestoredMessage(history, record, {
		.id = history->session().data().nextLocalMessageId(),
		.flags = MessageFlag::Local,
	});
	history->insertRestoredMessage(item);
	HistoryFeature::MarkDeletedInPlace(item);
}

void RestoreLoaded(gsl::not_null<::History*> history) {
	const auto session = &history->session();
	const auto store = HistoryFeature::StoreFor(session);
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

QString DeletedReplyText(
		gsl::not_null<::History*> history,
		qint64 messageId,
		const QString &fallback) {
	constexpr auto kPreviewLength = 64;
	const auto store = (messageId > 0)
		? HistoryFeature::StoreFor(&history->session())
		: nullptr;
	if (!store) {
		return fallback;
	}
	const auto records = store->deleted({
		.peerId = qint64(history->peer->id.value),
		.minMessageId = messageId,
		.maxMessageId = messageId,
		.limit = 1,
	});
	if (records.empty()) {
		return fallback;
	}
	const auto &record = records.front();
	const auto text = (record.text.isEmpty()
		? record.mediaSummary
		: record.text).simplified();
	return text.isEmpty()
		? fallback
		: (fallback + u": "_q + text.left(kPreviewLength));
}

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
