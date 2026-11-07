#include "serein/features/history/wire_cache.h"

#include "serein/core/options.h"
#include "serein/features/history/model/recorder.h"
#include "serein/features/history/wire.h"
#include "serein/hooks/history.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

#include <map>
#include <memory>
#include <unordered_map>

namespace Serein::HistoryFeature {
namespace {

struct Capture {
	std::unordered_map<FullMsgId, QByteArray> messages;
	std::set<qint64> excluded;
	bool enabled = false;
	rpl::lifetime lifetime;
};

using Captures = std::map<Main::Session*, std::unique_ptr<Capture>>;

[[nodiscard]] Captures &All() {
	static auto result = Captures();
	return result;
}

[[nodiscard]] Capture *Find(gsl::not_null<Main::Session*> session) {
	const auto &all = All();
	const auto i = all.find(session.get());
	return (i != end(all)) ? i->second.get() : nullptr;
}

[[nodiscard]] Capture *Wanted(gsl::not_null<::History*> history) {
	const auto capture = Find(&history->session());
	const auto peerId = qint64(history->peer->id.value);
	return (capture && capture->enabled && !capture->excluded.contains(peerId))
		? capture
		: nullptr;
}

void ApplyPolicy(gsl::not_null<Capture*> capture, const Policy &policy) {
	capture->enabled = policy.saveDeleted || policy.saveEdits;
	capture->excluded = policy.excludedPeers;
	if (!capture->enabled) {
		capture->messages.clear();
		return;
	}
	std::erase_if(capture->messages, [&](const auto &entry) {
		return capture->excluded.contains(qint64(entry.first.peer.value));
	});
}

} // namespace

void StartWireCapture(gsl::not_null<Main::Session*> session) {
	auto &slot = All()[session.get()];
	if (slot) {
		return;
	}
	slot = std::make_unique<Capture>();
	const auto capture = slot.get();
	const auto raw = session.get();
	session->lifetime().add([=] {
		All().erase(raw);
	});
	auto &account = ForAccount(session);
	using namespace HistorySettings;
	rpl::merge(
		account.Value(kHistorySaveDeleted) | rpl::to_empty,
		account.Value(kHistorySaveEdits) | rpl::to_empty,
		account.Value(kHistoryExcludedPeers) | rpl::to_empty
	) | rpl::on_next([=] {
		ApplyPolicy(capture, Read(ForAccount(raw)));
	}, capture->lifetime);
	session->data().itemRemoved(
	) | rpl::on_next([=](gsl::not_null<const HistoryItem*> item) {
		capture->messages.erase(item->fullId());
	}, capture->lifetime);
}

QByteArray CapturedWire(gsl::not_null<const HistoryItem*> item) {
	const auto capture = Find(&item->history()->session());
	if (!capture) {
		return QByteArray();
	}
	const auto i = capture->messages.find(item->fullId());
	return (i != end(capture->messages)) ? i->second : QByteArray();
}

} // namespace Serein::HistoryFeature

namespace Serein::Hooks {

template <typename Message>
void OnMessageReceived(
		gsl::not_null<::History*> history,
		qint64 id,
		const Message &message) {
	const auto msgId = MsgId(id);
	if (!IsServerMsgId(msgId)) {
		return;
	} else if (const auto capture = HistoryFeature::Wanted(history)) {
		const auto key = FullMsgId(history->peer->id, msgId);
		if (!capture->messages.contains(key)) {
			capture->messages.emplace(
				key,
				HistoryFeature::SerializeMessage(message));
		}
	}
}

template <typename Message>
void OnMessageEdited(
		gsl::not_null<HistoryItem*> item,
		const Message &message) {
	if (!IsServerMsgId(item->id)) {
		return;
	} else if (const auto capture = HistoryFeature::Wanted(item->history())) {
		capture->messages.insert_or_assign(
			item->fullId(),
			HistoryFeature::SerializeMessage(message));
	}
}

template void OnMessageReceived<MTPMessage>(
	gsl::not_null<::History*> history,
	qint64 id,
	const MTPMessage &message);
template void OnMessageEdited<MTPMessage>(
	gsl::not_null<HistoryItem*> item,
	const MTPMessage &message);

} // namespace Serein::Hooks
