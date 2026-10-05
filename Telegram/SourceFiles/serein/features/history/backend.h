#pragma once

#include "serein/features/history/model/recorder.h"
#include "base/basic_types.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <gsl/pointers>
#include <rpl/producer.h>

#include <memory>
#include <optional>

namespace Main {
class Session;
} // namespace Main

namespace Serein::HistoryFeature {

struct Storage {
	std::shared_ptr<Ports::Cipher> cipher;
	std::unique_ptr<Ports::HistoryStore> store;
};

struct StorageProvider {
	Fn<bool(const QString &directory)> exists;
	Fn<Storage(
		gsl::not_null<Main::Session*> session,
		const QString &directory)> open;
	Fn<void(const QString &directory)> remove;
};

void SetStorageProvider(StorageProvider provider);

struct Backend {
	std::shared_ptr<Ports::Cipher> cipher;
	std::unique_ptr<Ports::HistoryStore> store;
	std::unique_ptr<Recorder> recorder;
	QString mediaDirectory;
};

[[nodiscard]] Backend *BackendFor(
	gsl::not_null<Main::Session*> session,
	bool create);
[[nodiscard]] Ports::HistoryStore *StoreFor(
	gsl::not_null<Main::Session*> session);

void PruneHistory(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool ClearHistory(
	gsl::not_null<Main::Session*> session,
	qint64 peerId);

// Fires the peer id whose records changed, zero when every chat changed.
[[nodiscard]] rpl::producer<qint64> RecordsChanged(
	gsl::not_null<Main::Session*> session);
void NotifyRecordsChanged(
	gsl::not_null<Main::Session*> session,
	qint64 peerId);
[[nodiscard]] std::optional<QByteArray> CachedMediaBytes(
	gsl::not_null<Main::Session*> session,
	const History::Record &record);

} // namespace Serein::HistoryFeature
