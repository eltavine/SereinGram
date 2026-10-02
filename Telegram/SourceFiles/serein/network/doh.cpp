#include "serein/hooks/network/doh.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>

#include <algorithm>

namespace Serein::Network {
namespace {

constexpr auto kSystemDnsTimeToLive = crl::time(5 * 60 * 1000);

} // namespace

QStringList CustomDohHosts() {
	const auto host = ForDevice().Get(ServiceSettings::kCustomDoh);
	return host.isEmpty() ? QStringList() : QStringList{ host };
}

bool ResolveWithSystemDns(
		QObject *context,
		const QString &domain,
		const Fn<void(
			const QString &domain,
			const QStringList &ips,
			crl::time expireAt)> &done) {
	if (!ForDevice().Get(ServiceSettings::kSystemDns)) {
		return false;
	}
	QHostInfo::lookupHost(domain, context, [=](const QHostInfo &info) {
		auto addresses = info.addresses();
		std::stable_partition(
			addresses.begin(),
			addresses.end(),
			[](const QHostAddress &address) {
				return address.protocol() == QAbstractSocket::IPv4Protocol;
			});
		auto ips = QStringList();
		for (const auto &address : std::as_const(addresses)) {
			ips.push_back(address.toString());
		}
		if (!ips.isEmpty()) {
			done(domain, ips, crl::now() + kSystemDnsTimeToLive);
		}
	});
	return true;
}

} // namespace Serein::Network
