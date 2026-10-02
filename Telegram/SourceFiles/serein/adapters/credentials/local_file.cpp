#include "serein/ports/credentials.h"

#include "serein/adapters/openssl/aes_gcm_cipher.h"
#include "core/application.h"
#include "main/main_account.h"
#include "mtproto/mtproto_auth_key.h"
#include "settings.h"
#include "storage/storage_account.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>

namespace Serein::Ports::SecretStore {
namespace {

constexpr auto kMaximumSize = 1024 * 1024;

[[nodiscard]] QString StorePath() {
	return cWorkingDir() + u"tdata/serein_credentials"_q;
}

[[nodiscard]] std::unique_ptr<Adapters::AesGcmCipher> MakeCipher() {
	if (!Core::IsAppLaunched()) {
		return nullptr;
	}
	const auto key = Core::App().activeAccount().local().peekLegacyLocalKey();
	if (!key) {
		return nullptr;
	}
	const auto bytes = key->data();
	return Adapters::AesGcmCipher::FromSecret(
		QByteArray(
			reinterpret_cast<const char*>(bytes.data()),
			int(bytes.size())),
		"serein-credentials-v1");
}

[[nodiscard]] std::optional<QJsonObject> Load() {
	auto file = QFile(StorePath());
	if (!file.exists()) {
		return QJsonObject();
	} else if (file.size() > kMaximumSize || !file.open(QIODevice::ReadOnly)) {
		return std::nullopt;
	}
	const auto document = QJsonDocument::fromJson(file.readAll());
	return document.isObject()
		? std::make_optional(document.object())
		: std::nullopt;
}

[[nodiscard]] bool Save(const QJsonObject &entries) {
	if (entries.isEmpty()) {
		return !QFile::exists(StorePath()) || QFile::remove(StorePath());
	}
	const auto bytes = QJsonDocument(entries).toJson(QJsonDocument::Compact);
	auto file = QSaveFile(StorePath());
	if (!file.open(QIODevice::WriteOnly)) {
		return false;
	} else if (file.write(bytes) != bytes.size()) {
		file.cancelWriting();
		return false;
	}
	return file.commit();
}

} // namespace

CredentialResult Read(const QString &account) {
	const auto cipher = MakeCipher();
	const auto entries = Load();
	if (!cipher || !entries) {
		return { .error = CredentialError::Unavailable };
	}
	const auto value = entries->value(account);
	if (!value.isString()) {
		return { .error = CredentialError::Missing };
	}
	const auto decoded = QByteArray::fromBase64Encoding(
		value.toString().toLatin1());
	auto secret = decoded
		? cipher->decrypt(decoded.decoded)
		: std::nullopt;
	if (!secret) {
		return { .error = CredentialError::Invalid };
	}
	return { .secret = std::move(*secret) };
}

CredentialError Write(const QString &account, const QByteArray &secret) {
	const auto cipher = MakeCipher();
	auto entries = Load();
	if (!cipher || !entries) {
		return CredentialError::Unavailable;
	}
	const auto sealed = cipher->encrypt(secret);
	if (sealed.isEmpty()) {
		return CredentialError::Denied;
	}
	entries->insert(account, QString::fromLatin1(sealed.toBase64()));
	return Save(*entries) ? CredentialError::None : CredentialError::Denied;
}

CredentialError Remove(const QString &account) {
	auto entries = Load();
	if (!entries) {
		return CredentialError::Unavailable;
	} else if (!entries->contains(account)) {
		return CredentialError::None;
	}
	entries->remove(account);
	return Save(*entries) ? CredentialError::None : CredentialError::Denied;
}

} // namespace Serein::Ports::SecretStore
