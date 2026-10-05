#include "serein/features/history/recording.h"

#include "serein/hooks/history.h"
#include "serein/core/options.h"
#include "serein/features/history/backend.h"
#include "serein/features/history/capture.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/media_store.h"
#include "data/data_changes.h"
#include "data/data_channel.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "logs.h"
#include "main/main_session.h"
#include "ui/text/text_entity.h"

#include <crl/crl_async.h>

#include <set>
#include <utility>

namespace Serein::HistoryFeature {
namespace {

class Batch final {
public:
	explicit Batch(Ports::HistoryStore *store) : _store(store) {
		if (_store) {
			_store->beginBatch();
		}
	}
	~Batch() {
		if (_store) {
			_store->endBatch();
		}
	}
	Batch(const Batch &) = delete;
	Batch &operator=(const Batch &) = delete;

private:
	Ports::HistoryStore *_store = nullptr;

};

bool RecordDeleted(
		gsl::not_null<Main::Session*> session,
		const Policy &policy,
		gsl::not_null<HistoryItem*> item) {
	const auto backend = BackendFor(session, true);
	if (!backend) {
		return false;
	}
	auto snapshot = TakeSnapshot(item);
	auto media = snapshot.localPath.isEmpty()
		? CaptureCachedMedia(item)
		: std::nullopt;
	if (media) {
		snapshot.cachedMediaName = media->name;
	}
	if (!backend->recorder->recordDeleted(policy, snapshot)) {
		return false;
	} else if (!media) {
		return true;
	}
	const auto path = CachedMediaPath(
		backend->mediaDirectory,
		snapshot.peerId,
		snapshot.messageId);
	crl::async([
			cipher = backend->cipher,
			path,
			bytes = std::move(media->bytes)] {
		if (!WriteCachedMedia(*cipher, path, bytes)) {
			LOG(("Serein History: could not cache deleted media in %1.").arg(path));
		}
	});
	return true;
}

} // namespace

void WatchRemovedChats(gsl::not_null<Main::Session*> session) {
	session->changes().peerUpdates(
		Data::PeerUpdate::Flag::ChannelAmIn
	) | rpl::on_next([=](const Data::PeerUpdate &update) {
		const auto channel = update.peer->asChannel();
		if (!channel || !channel->isForbidden()) {
			return;
		}
		const auto policy = Read(ForAccount(session));
		const auto history = session->data().historyLoaded(channel);
		if (!policy.saveDeleted || !policy.keepRemovedChats || !history) {
			return;
		}
		const auto backend = BackendFor(session, true);
		auto recorded = false;
		{
			const auto batch = Batch(backend ? backend->store.get() : nullptr);
			for (const auto &block : history->blocks) {
				for (const auto &view : block->messages) {
					const auto item = view->data();
					if (item->isRegular() && !item->isService()) {
						recorded |= RecordDeleted(session, policy, item);
					}
				}
			}
		}
		if (recorded) {
			NotifyRecordsChanged(session, qint64(channel->id.value));
		}
	}, session->lifetime());
}

} // namespace Serein::HistoryFeature

namespace Serein::Hooks {

std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	auto batch = std::optional<HistoryFeature::Batch>();
	auto batchSession = (Main::Session*)nullptr;
	auto changed = std::set<std::pair<Main::Session*, qint64>>();
	for (const auto &item : items) {
		const auto session = &item->history()->session();
		const auto policy = HistoryFeature::Read(ForAccount(session));
		if (policy.saveDeleted) {
			if (batchSession != session) {
				batch.reset();
				batchSession = session;
				const auto backend = HistoryFeature::BackendFor(session, true);
				batch.emplace(backend ? backend->store.get() : nullptr);
			}
			if (HistoryFeature::RecordDeleted(session, policy, item)) {
				changed.emplace(
					session,
					qint64(item->history()->peer->id.value));
			}
		}
	}
	batch.reset();
	for (const auto &[session, peerId] : changed) {
		HistoryFeature::NotifyRecordsChanged(session, peerId);
	}
	return HistoryFeature::KeepDeletedInPlace(std::move(items));
}

void OnBeforeEdition(
		gsl::not_null<HistoryItem*> item,
		const TextWithEntities &updated) {
	const auto session = &item->history()->session();
	const auto policy = HistoryFeature::Read(ForAccount(session));
	if (!policy.saveEdits || item->originalText() == updated) {
		return;
	} else if (const auto backend = HistoryFeature::BackendFor(session, true)) {
		const auto recorded = backend->recorder->recordEdit(
			policy,
			HistoryFeature::TakeSnapshot(item));
		if (recorded) {
			HistoryFeature::NotifyRecordsChanged(
				session,
				qint64(item->history()->peer->id.value));
		}
	}
}

} // namespace Serein::Hooks
