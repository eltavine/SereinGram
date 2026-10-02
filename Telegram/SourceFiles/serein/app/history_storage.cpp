#include "serein/app/history_storage.h"

#include "serein/adapters/openssl/aes_gcm_cipher.h"
#include "serein/adapters/qtsql/history_store.h"
#include "serein/features/history/backend.h"
#include "logs.h"
#include "main/main_session.h"
#include "mtproto/mtproto_auth_key.h"
#include "storage/storage_account.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace Serein::App {
namespace {

[[nodiscard]] QString DatabasePath(const QString &directory) {
	return directory + u"/serein_history.sqlite3"_q;
}

[[nodiscard]] HistoryFeature::Storage Open(
		gsl::not_null<Main::Session*> session,
		const QString &directory) {
	const auto key = session->local().peekLegacyLocalKey();
	if (!key) {
		return {};
	}
	const auto bytes = key->data();
	auto cipher = std::shared_ptr<Adapters::AesGcmCipher>(
		Adapters::AesGcmCipher::FromSecret(
			QByteArray(
				reinterpret_cast<const char*>(bytes.data()),
				int(bytes.size())),
			"serein-history-v1"));
	if (!cipher) {
		return {};
	}
	auto error = QString();
	auto store = Adapters::SqlHistoryStore::Open(
		DatabasePath(directory),
		*cipher,
		&error);
	if (!store) {
		LOG(("Serein History Error: %1").arg(error));
		return {};
	}
	return { std::move(cipher), std::move(store) };
}

void Remove(const QString &directory) {
	const auto path = DatabasePath(directory);
	for (const auto &suffix : { u""_q, u"-journal"_q, u"-wal"_q, u"-shm"_q }) {
		QFile::remove(path + suffix);
	}
}

} // namespace

void RegisterHistoryStorage() {
	HistoryFeature::SetStorageProvider({
		.exists = [](const QString &directory) {
			return QFileInfo::exists(DatabasePath(directory));
		},
		.open = Open,
		.remove = Remove,
	});
}

} // namespace Serein::App
