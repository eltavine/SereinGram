#pragma once

#include "serein/ports/credentials.h"

namespace Serein {

[[nodiscard]] CredentialResult ReadCredential(const QString &account);
[[nodiscard]] CredentialError WriteCredential(
	const QString &account,
	const QByteArray &secret);
[[nodiscard]] CredentialError DeleteCredential(const QString &account);

} // namespace Serein
