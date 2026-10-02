#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace Serein {

enum class CredentialError {
	None,
	Missing,
	Unavailable,
	Denied,
	Invalid,
};

struct CredentialResult {
	QByteArray secret;
	CredentialError error = CredentialError::None;
};

} // namespace Serein

namespace Serein::Ports::SecretStore {

[[nodiscard]] CredentialResult Read(const QString &account);
[[nodiscard]] CredentialError Write(
	const QString &account,
	const QByteArray &secret);
[[nodiscard]] CredentialError Remove(const QString &account);

} // namespace Serein::Ports::SecretStore
