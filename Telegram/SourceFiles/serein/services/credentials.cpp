#include "serein/services/credentials.h"

namespace Serein {
namespace {

constexpr auto kMaxAccountSize = 256;
constexpr auto kMaxSecretSize = 4096;

[[nodiscard]] bool ValidAccount(const QString &account) {
	return !account.isEmpty()
		&& account.size() <= kMaxAccountSize
		&& !account.contains(QChar(0));
}

[[nodiscard]] bool ValidSecret(const QByteArray &secret) {
	return !secret.isEmpty()
		&& secret.size() <= kMaxSecretSize
		&& !secret.contains('\0')
		&& !secret.contains('\r')
		&& !secret.contains('\n');
}

} // namespace

CredentialResult ReadCredential(const QString &account) {
	if (!ValidAccount(account)) {
		return { .error = CredentialError::Invalid };
	}
	return Ports::SecretStore::Read(account);
}

CredentialError WriteCredential(
		const QString &account,
		const QByteArray &secret) {
	if (!ValidAccount(account) || !ValidSecret(secret)) {
		return CredentialError::Invalid;
	}
	return Ports::SecretStore::Write(account, secret);
}

CredentialError DeleteCredential(const QString &account) {
	if (!ValidAccount(account)) {
		return CredentialError::Invalid;
	}
	return Ports::SecretStore::Remove(account);
}

} // namespace Serein
