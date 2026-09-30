#include "serein/hooks/history.h"

#include "serein/adapters/openssl/aes_gcm_cipher.h"
#include "serein/adapters/qtsql/history_store.h"
#include "serein/app/history_entities.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/model/recorder.h"
#include "base/unixtime.h"
#include "core/application.h"
#include "data/data_document.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "logs.h"
#include "main/main_session.h"
#include "mtproto/mtproto_auth_key.h"
#include "storage/storage_account.h"
#include "ui/text/text_entity.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <map>

namespace Serein::Hooks {
namespace {

struct Backend {
	std::unique_ptr<Adapters::AesGcmCipher> cipher;
	std::unique_ptr<Adapters::SqlHistoryStore> store;
	std::unique_ptr<HistoryFeature::Recorder> recorder;
};

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
	session->lifetime().add([=, path = DatabasePath(session)] {
		backends.erase(session);
		if (!Core::Quitting()) {
			RemoveDatabase(path);
		}
	});
	return slot.get();
}

[[nodiscard]] HistoryFeature::Recorder *RecorderFor(
		not_null<Main::Session*> session) {
	const auto backend = BackendFor(session, true);
	return backend ? backend->recorder.get() : nullptr;
}

} // namespace

std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
		std::vector<gsl::not_null<HistoryItem*>> items) {
	for (const auto &item : items) {
		const auto session = &item->history()->session();
		const auto policy = HistoryFeature::Read(ForAccount(session));
		if (!policy.saveDeleted) {
			continue;
		} else if (const auto recorder = RecorderFor(session)) {
			recorder->recordDeleted(policy, TakeSnapshot(item));
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
	}
}

} // namespace Serein::Hooks
