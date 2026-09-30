#include "serein/hooks/history.h"

#include "serein/adapters/openssl/aes_gcm_cipher.h"
#include "serein/adapters/qtsql/history_store.h"
#include "serein/app/history_entities.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/media_store.h"
#include "serein/features/history/model/recorder.h"
#include "base/unixtime.h"
#include "core/application.h"
#include "core/file_utilities.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_photo.h"
#include "data/data_photo_media.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "logs.h"
#include "main/main_session.h"
#include "mtproto/mtproto_auth_key.h"
#include "storage/storage_account.h"
#include "ui/text/text_entity.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

#include <map>

namespace Serein::Hooks {
namespace {

struct Backend {
	std::unique_ptr<Adapters::AesGcmCipher> cipher;
	std::unique_ptr<Adapters::SqlHistoryStore> store;
	std::unique_ptr<HistoryFeature::Recorder> recorder;
};

struct CachedMedia {
	QByteArray bytes;
	QString name;
};

[[nodiscard]] QString SafeName(const QString &name) {
	auto result = QFileInfo(name).fileName().left(255);
	for (auto &ch : result) {
		if (ch < QChar(0x20) || QStringView(u"<>:\"/\\|?*").contains(ch)) {
			ch = u'_';
		}
	}
	return result;
}

[[nodiscard]] std::optional<CachedMedia> CaptureCachedMedia(
		not_null<HistoryItem*> item) {
	const auto media = item->media();
	if (const auto photo = media ? media->photo() : nullptr) {
		const auto view = photo->activeMediaView();
		auto bytes = view
			? view->imageBytes(Data::PhotoSize::Large)
			: QByteArray();
		if (!bytes.isEmpty()) {
			return CachedMedia{
				std::move(bytes),
				u"photo_%1.jpg"_q.arg(item->id.bare),
			};
		}
	} else if (const auto document = media ? media->document() : nullptr) {
		const auto view = document->activeMediaView();
		auto bytes = view ? view->bytes() : QByteArray();
		if (!bytes.isEmpty()) {
			auto name = SafeName(document->filename());
			if (name.isEmpty()) {
				const auto suffix = QMimeDatabase().mimeTypeForName(
					document->mimeString()).preferredSuffix();
				name = u"file_%1"_q.arg(item->id.bare)
					+ (suffix.isEmpty() ? QString() : (u'.' + suffix));
			}
			return CachedMedia{ std::move(bytes), name };
		}
	}
	return std::nullopt;
}

[[nodiscard]] HistoryFeature::Snapshot TakeSnapshot(not_null<HistoryItem*> item) {
	auto result = HistoryFeature::Snapshot();
	result.peerId = qint64(item->history()->peer->id.value);
	result.messageId = item->id.bare;
	result.topicRootId = item->topicRootId().bare;
	result.date = item->date();
	const auto from = item->from();
	result.fromPeerId = qint64(from->id.value);
	const auto user = from->asUser();
	result.fromBot = user && user->isBot();
	const auto &text = item->originalText();
	result.text = text.text;
	for (const auto &entity : text.entities) {
		const auto name = App::EntityName(entity.type());
		if (!name.isEmpty() && entity.offset() >= 0 && entity.length() > 0) {
			result.entities.push_back({
				name,
				entity.offset(),
				entity.length(),
				entity.data(),
			});
		}
	}
	if (const auto media = item->media()) {
		result.mediaSummary = media->notificationText().text;
		if (const auto document = media->document()) {
			result.localPath = document->filepath(true);
		}
	}
	return result;
}

[[nodiscard]] QString DatabasePath(not_null<Main::Session*> session) {
	return QFileInfo(session->local().supportModePath()).absolutePath()
		+ u"/serein_history.sqlite3"_q;
}

[[nodiscard]] QString MediaDirectory(not_null<Main::Session*> session) {
	return QFileInfo(session->local().supportModePath()).absolutePath()
		+ u"/serein_media"_q;
}

void RemoveDatabase(const QString &path) {
	for (const auto &suffix : { u""_q, u"-journal"_q, u"-wal"_q, u"-shm"_q }) {
		QFile::remove(path + suffix);
	}
}

[[nodiscard]] std::unique_ptr<Backend> OpenBackend(
		not_null<Main::Session*> session) {
	const auto key = session->local().peekLegacyLocalKey();
	if (!key) {
		return nullptr;
	}
	const auto bytes = key->data();
	auto backend = std::make_unique<Backend>();
	backend->cipher = Adapters::AesGcmCipher::FromSecret(
		QByteArray(reinterpret_cast<const char*>(bytes.data()), int(bytes.size())),
		"serein-history-v1");
	if (!backend->cipher) {
		return nullptr;
	}
	auto error = QString();
	backend->store = Adapters::SqlHistoryStore::Open(
		DatabasePath(session),
		*backend->cipher,
		&error);
	if (!backend->store) {
		LOG(("Serein History Error: %1").arg(error));
		return nullptr;
	}
	backend->recorder = std::make_unique<HistoryFeature::Recorder>(
		*backend->store,
		[] { return qint64(base::unixtime::now()); });
	backend->recorder->prune(HistoryFeature::Read(ForAccount(session)));
	HistoryFeature::RemoveOrphanedCachedMedia(
		MediaDirectory(session),
		*backend->store);
	return backend;
}

[[nodiscard]] Backend *BackendFor(
		not_null<Main::Session*> session,
		bool create) {
	static auto backends = std::map<Main::Session*, std::unique_ptr<Backend>>();
	if (const auto i = backends.find(session); i != backends.end()) {
		return i->second.get();
	} else if (!create && !QFileInfo::exists(DatabasePath(session))) {
		return nullptr;
	}
	auto &slot = backends[session];
	slot = OpenBackend(session);
	session->lifetime().add([=,
			path = DatabasePath(session),
			media = MediaDirectory(session)] {
		backends.erase(session);
		if (!Core::Quitting()) {
			RemoveDatabase(path);
			HistoryFeature::RemoveCachedMedia(media, 0);
		}
	});
	return slot.get();
}

[[nodiscard]] HistoryFeature::Recorder *RecorderFor(
		not_null<Main::Session*> session) {
	const auto backend = BackendFor(session, true);
	return backend ? backend->recorder.get() : nullptr;
}

void RecordDeleted(
		not_null<Main::Session*> session,
		const HistoryFeature::Policy &policy,
		not_null<HistoryItem*> item) {
	const auto backend = BackendFor(session, true);
	if (!backend) {
		return;
	}
	auto snapshot = TakeSnapshot(item);
	const auto path = HistoryFeature::CachedMediaPath(
		MediaDirectory(session),
		snapshot.peerId,
		snapshot.messageId);
	auto written = false;
	if (snapshot.localPath.isEmpty()) {
		if (const auto media = CaptureCachedMedia(item)) {
			written = HistoryFeature::WriteCachedMedia(
				*backend->cipher,
				path,
				media->bytes);
			if (written) {
				snapshot.cachedMediaName = media->name;
			}
		}
	}
	if (!backend->recorder->recordDeleted(policy, snapshot) && written) {
		QFile::remove(path);
	}
}

} // namespace

std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	for (const auto &item : items) {
		const auto session = &item->history()->session();
		const auto policy = HistoryFeature::Read(ForAccount(session));
		if (policy.saveDeleted) {
			RecordDeleted(session, policy, item);
		}
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
	} else if (const auto recorder = RecorderFor(session)) {
		recorder->recordEdit(policy, TakeSnapshot(item));
	}
}

Ports::HistoryStore *HistoryStoreFor(gsl::not_null<Main::Session*> session) {
	const auto backend = BackendFor(session, false);
	return backend ? backend->store.get() : nullptr;
}

void PruneHistory(gsl::not_null<Main::Session*> session) {
	if (const auto backend = BackendFor(session, false)) {
		backend->recorder->prune(HistoryFeature::Read(ForAccount(session)));
		HistoryFeature::RemoveOrphanedCachedMedia(
			MediaDirectory(session),
			*backend->store);
	}
}

bool ClearHistory(gsl::not_null<Main::Session*> session, long long peerId) {
	const auto backend = BackendFor(session, false);
	if (!backend) {
		return true;
	}
	const auto cleared = peerId
		? backend->store->clearPeer(peerId)
		: backend->store->clearAll();
	if (cleared) {
		HistoryFeature::RemoveCachedMedia(MediaDirectory(session), peerId);
	}
	return cleared;
}

bool OpenCachedMedia(
		gsl::not_null<Main::Session*> session,
		const Serein::History::Record &record) {
	const auto backend = BackendFor(session, false);
	const auto name = SafeName(record.cachedMediaName);
	if (!backend || name.isEmpty()) {
		return false;
	}
	const auto bytes = HistoryFeature::ReadCachedMedia(
		*backend->cipher,
		HistoryFeature::CachedMediaPath(
			MediaDirectory(session),
			record.peerId,
			record.messageId));
	const auto folder = QDir::temp().filePath(u"SereinGram"_q);
	const auto path = QDir(folder).filePath(name);
	auto file = QFile(path);
	if (!bytes
		|| !QDir().mkpath(folder)
		|| !file.open(QIODevice::WriteOnly)
		|| file.write(*bytes) != bytes->size()) {
		return false;
	}
	file.close();
	File::Launch(path);
	return true;
}

} // namespace Serein::Hooks
