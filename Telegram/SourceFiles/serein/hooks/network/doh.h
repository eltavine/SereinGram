#pragma once

#include "base/basic_types.h"

#include <QtCore/QStringList>

class QObject;

namespace Serein::Network {

[[nodiscard]] QStringList CustomDohHosts();

template <typename Attempts, typename Type>
void PrependCustomDoh(Attempts &attempts, Type type) {
	for (const auto &host : CustomDohHosts()) {
		attempts.insert(
			attempts.begin(),
			typename Attempts::value_type{ type, host });
	}
}

[[nodiscard]] bool ResolveWithSystemDns(
	QObject *context,
	const QString &domain,
	const Fn<void(
		const QString &domain,
		const QStringList &ips,
		crl::time expireAt)> &done);

} // namespace Serein::Network
