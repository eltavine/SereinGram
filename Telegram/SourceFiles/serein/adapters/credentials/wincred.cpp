#include "serein/ports/credentials.h"

#include <windows.h>
#include <wincred.h>

namespace Serein::Ports::SecretStore {
namespace {

[[nodiscard]] QString Target(const QString &account) {
	return u"SereinGram Services/"_q + account;
}

[[nodiscard]] LPCWSTR Name(const QString &target) {
	return reinterpret_cast<LPCWSTR>(target.utf16());
}

} // namespace

CredentialResult Read(const QString &account) {
	const auto target = Target(account);
	auto credential = PCREDENTIALW(nullptr);
	if (!CredReadW(Name(target), CRED_TYPE_GENERIC, 0, &credential)) {
		return {
			.error = (GetLastError() == ERROR_NOT_FOUND)
				? CredentialError::Missing
				: CredentialError::Denied,
		};
	}
	auto secret = QByteArray(
		reinterpret_cast<const char*>(credential->CredentialBlob),
		qsizetype(credential->CredentialBlobSize));
	CredFree(credential);
	return { .secret = std::move(secret) };
}

CredentialError Write(const QString &account, const QByteArray &secret) {
	const auto target = Target(account);
	auto credential = CREDENTIALW{};
	credential.Type = CRED_TYPE_GENERIC;
	credential.TargetName = const_cast<LPWSTR>(Name(target));
	credential.CredentialBlobSize = DWORD(secret.size());
	credential.CredentialBlob = reinterpret_cast<LPBYTE>(
		const_cast<char*>(secret.constData()));
	credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
	return CredWriteW(&credential, 0)
		? CredentialError::None
		: CredentialError::Denied;
}

CredentialError Remove(const QString &account) {
	const auto target = Target(account);
	return (CredDeleteW(Name(target), CRED_TYPE_GENERIC, 0)
		|| GetLastError() == ERROR_NOT_FOUND)
		? CredentialError::None
		: CredentialError::Denied;
}

} // namespace Serein::Ports::SecretStore
