#include "serein/features/history/backend.h"

#include "serein/core/options.h"
#include "serein/features/history/media_store.h"
#include "base/unixtime.h"
#include "core/application.h"
#include "main/main_session.h"
#include "storage/storage_account.h"

#include <QtCore/QFileInfo>
#include <rpl/event_stream.h>

#include <map>

namespace Serein::HistoryFeature {
namespace {

using BackendMap = std::map<Main::Session*, std::unique_ptr<Backend>>;

[[nodiscard]] StorageProvider &Provider() {
	static auto result = StorageProvider();
	return result;
}

[[nodiscard]] BackendMap &Backends() {
	static auto result = BackendMap();
	return result;
}

using ChangesMap = std::map<Main::Session*, rpl::event_stream<qint64>>;

[[nodiscard]] ChangesMap &Changes() {
	static auto result = ChangesMap();
	return result;
}

[[nodiscard]] QString AccountDirectory(gsl::not_null<Main::Session*> session) {
	return QFileInfo(session->local().supportModePath()).absolutePath();
}

[[nodiscard]] QString MediaDirectory(const QString &directory) {
	return directory + u"/serein_media"_q;
}

[[nodiscard]] std::unique_ptr<Backend> Open(
		gsl::not_null<Main::Session*> session,
		const QString &directory) {
	auto storage = Provider().open(session, directory);
	if (!storage.cipher || !storage.store) {
		return nullptr;
	}
	auto result = std::make_unique<Backend>();
	result->cipher = std::move(storage.cipher);
	result->store = std::move(storage.store);
	result->recorder = std::make_unique<Recorder>(
		*result->store,
		[] { return qint64(base::unixtime::now()); });
	result->mediaDirectory = MediaDirectory(directory);
	result->recorder->prune(Read(ForAccount(session)));
	RemoveOrphanedCachedMedia(result->mediaDirectory, *result->store);
	return result;
}

} // namespace

void SetStorageProvider(StorageProvider provider) {
	Provider() = std::move(provider);
}

Backend *BackendFor(gsl::not_null<Main::Session*> session, bool create) {
	auto &backends = Backends();
	if (const auto i = backends.find(session); i != backends.end()) {
		return i->second.get();
	}
	const auto &provider = Provider();
	const auto directory = AccountDirectory(session);
	if (!provider.open || (!create && !provider.exists(directory))) {
		return nullptr;
	}
	auto &slot = backends[session];
	slot = Open(session, directory);
	const auto raw = session.get();
	session->lifetime().add([=] {
		Backends().erase(raw);
		if (Core::Quitting()) {
			return;
		} else if (const auto &remove = Provider().remove) {
			remove(directory);
		}
		RemoveCachedMedia(MediaDirectory(directory), 0);
	});
	return slot.get();
}

Ports::HistoryStore *StoreFor(gsl::not_null<Main::Session*> session) {
	const auto backend = BackendFor(session, false);
	return backend ? backend->store.get() : nullptr;
}

void PruneHistory(gsl::not_null<Main::Session*> session) {
	if (const auto backend = BackendFor(session, false)) {
		backend->recorder->prune(Read(ForAccount(session)));
		RemoveOrphanedCachedMedia(backend->mediaDirectory, *backend->store);
	}
}

bool ClearHistory(gsl::not_null<Main::Session*> session, qint64 peerId) {
	const auto backend = BackendFor(session, false);
	if (!backend) {
		return true;
	}
	const auto cleared = peerId
		? backend->store->clearPeer(peerId)
		: backend->store->clearAll();
	if (cleared) {
		RemoveCachedMedia(backend->mediaDirectory, peerId);
		NotifyRecordsChanged(session, peerId);
	}
	return cleared;
}

rpl::producer<qint64> RecordsChanged(gsl::not_null<Main::Session*> session) {
	const auto raw = session.get();
	const auto [i, fresh] = Changes().try_emplace(raw);
	if (fresh) {
		session->lifetime().add([=] {
			Changes().erase(raw);
		});
	}
	return i->second.events();
}

void NotifyRecordsChanged(
		gsl::not_null<Main::Session*> session,
		qint64 peerId) {
	const auto &changes = Changes();
	if (const auto i = changes.find(session.get()); i != end(changes)) {
		i->second.fire_copy(peerId);
	}
}

std::optional<QByteArray> CachedMediaBytes(
		gsl::not_null<Main::Session*> session,
		const History::Record &record) {
	const auto backend = BackendFor(session, false);
	if (!backend || record.cachedMediaName.isEmpty()) {
		return std::nullopt;
	}
	return ReadCachedMedia(
		*backend->cipher,
		CachedMediaPath(
			backend->mediaDirectory,
			record.peerId,
			record.messageId));
}

} // namespace Serein::HistoryFeature
