#pragma once

#include "serein/services/credentials.h"

namespace Serein::LocalCredentials {

[[nodiscard]] CredentialResult Read(const QString &account);
[[nodiscard]] CredentialError Write(
	const QString &account,
	const QByteArray &secret);
[[nodiscard]] CredentialError Remove(const QString &account);

} // namespace Serein::LocalCredentials
