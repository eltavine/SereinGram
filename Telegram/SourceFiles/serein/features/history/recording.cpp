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

namespace Serein::HistoryFeature {
namespace {

constexpr auto kRemovedChatLimit = 500;

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

void RecordDeleted(
		gsl::not_null<Main::Session*> session,
		const Policy &policy,
		gsl::not_null<HistoryItem*> item) {
	const auto backend = BackendFor(session, true);
	if (!backend) {
		return;
	}
	auto snapshot = TakeSnapshot(item);
	auto media = snapshot.localPath.isEmpty()
		? CaptureCachedMedia(item)
		: std::nullopt;
	if (media) {
		snapshot.cachedMediaName = media->name;
	}
	if (!backend->recorder->recordDeleted(policy, snapshot) || !media) {
		return;
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
		auto left = kRemovedChatLimit;
		for (auto i = history->blocks.rbegin(); i != history->blocks.rend(); ++i) {
			const auto &messages = (*i)->messages;
			for (auto j = messages.rbegin(); j != messages.rend(); ++j) {
				const auto item = (*j)->data();
				if (!left) {
					return;
				} else if (item->isRegular() && !item->isService()) {
					RecordDeleted(session, policy, item);
					--left;
				}
			}
		}
	}, session->lifetime());
}

} // namespace Serein::HistoryFeature

namespace Serein::Hooks {

std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	auto batch = std::optional<HistoryFeature::Batch>();
	auto batchSession = (Main::Session*)nullptr;
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
			HistoryFeature::RecordDeleted(session, policy, item);
		}
	}
	batch.reset();
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
		backend->recorder->recordEdit(
			policy,
			HistoryFeature::TakeSnapshot(item));
	}
}

} // namespace Serein::Hooks
