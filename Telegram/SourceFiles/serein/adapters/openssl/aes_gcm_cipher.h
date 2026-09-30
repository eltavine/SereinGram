#pragma once

#include "serein/ports/history_store.h"

#include <memory>

namespace Serein::Adapters {

class AesGcmCipher final : public Ports::Cipher {
public:
	[[nodiscard]] static std::unique_ptr<AesGcmCipher> FromSecret(
		const QByteArray &secret,
		const QByteArray &context);

	[[nodiscard]] QByteArray encrypt(const QByteArray &plain) override;
	[[nodiscard]] std::optional<QByteArray> decrypt(
		const QByteArray &sealed) override;

private:
	explicit AesGcmCipher(QByteArray key);

	QByteArray _key;
};

} // namespace Serein::Adapters
