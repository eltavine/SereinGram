#include "serein/adapters/openssl/aes_gcm_cipher.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QRandomGenerator>

#include <openssl/evp.h>

namespace Serein::Adapters {
namespace {

constexpr auto kFormat = char(1);
constexpr auto kNonceSize = 12;
constexpr auto kTagSize = 16;

struct ContextDeleter {
	void operator()(EVP_CIPHER_CTX *context) const {
		EVP_CIPHER_CTX_free(context);
	}
};
using Context = std::unique_ptr<EVP_CIPHER_CTX, ContextDeleter>;

[[nodiscard]] const unsigned char *Bytes(const QByteArray &data) {
	return reinterpret_cast<const unsigned char*>(data.constData());
}

[[nodiscard]] unsigned char *Bytes(QByteArray &data) {
	return reinterpret_cast<unsigned char*>(data.data());
}

} // namespace

std::unique_ptr<AesGcmCipher> AesGcmCipher::FromSecret(
		const QByteArray &secret,
		const QByteArray &context) {
	if (secret.size() < 32 || context.isEmpty()) {
		return nullptr;
	}
	auto hash = QCryptographicHash(QCryptographicHash::Sha256);
	hash.addData(context);
	hash.addData(QByteArray(1, '\0'));
	hash.addData(secret);
	return std::unique_ptr<AesGcmCipher>(new AesGcmCipher(hash.result()));
}

AesGcmCipher::AesGcmCipher(QByteArray key) : _key(std::move(key)) {
}

QByteArray AesGcmCipher::encrypt(const QByteArray &plain) {
	auto result = QByteArray(1 + kNonceSize + plain.size() + kTagSize, '\0');
	result[0] = kFormat;
	auto nonce = result.mid(1, kNonceSize);
	QRandomGenerator::system()->generate(nonce.begin(), nonce.end());
	result.replace(1, kNonceSize, nonce);
	const auto context = Context(EVP_CIPHER_CTX_new());
	auto written = 0;
	auto finished = 0;
	auto tag = QByteArray(kTagSize, '\0');
	auto body = QByteArray(plain.size(), '\0');
	const auto ok = context
		&& EVP_EncryptInit_ex(context.get(), EVP_aes_256_gcm(), nullptr, Bytes(_key), Bytes(nonce))
		&& (plain.isEmpty() || EVP_EncryptUpdate(context.get(), Bytes(body), &written, Bytes(plain), plain.size()))
		&& EVP_EncryptFinal_ex(context.get(), Bytes(body) + written, &finished)
		&& EVP_CIPHER_CTX_ctrl(context.get(), EVP_CTRL_GCM_GET_TAG, kTagSize, Bytes(tag));
	if (!ok) {
		return QByteArray();
	}
	result.replace(1 + kNonceSize, plain.size(), body);
	result.replace(1 + kNonceSize + plain.size(), kTagSize, tag);
	return result;
}

std::optional<QByteArray> AesGcmCipher::decrypt(const QByteArray &sealed) {
	if (sealed.size() < 1 + kNonceSize + kTagSize || sealed[0] != kFormat) {
		return std::nullopt;
	}
	const auto size = sealed.size() - 1 - kNonceSize - kTagSize;
	const auto nonce = sealed.mid(1, kNonceSize);
	const auto body = sealed.mid(1 + kNonceSize, size);
	auto tag = sealed.right(kTagSize);
	auto result = QByteArray(size, '\0');
	const auto context = Context(EVP_CIPHER_CTX_new());
	auto written = 0;
	auto finished = 0;
	const auto ok = context
		&& EVP_DecryptInit_ex(context.get(), EVP_aes_256_gcm(), nullptr, Bytes(_key), Bytes(nonce))
		&& (body.isEmpty() || EVP_DecryptUpdate(context.get(), Bytes(result), &written, Bytes(body), body.size()))
		&& EVP_CIPHER_CTX_ctrl(context.get(), EVP_CTRL_GCM_SET_TAG, kTagSize, Bytes(tag))
		&& EVP_DecryptFinal_ex(context.get(), Bytes(result) + written, &finished) > 0;
	if (!ok) {
		return std::nullopt;
	}
	return result;
}

} // namespace Serein::Adapters
