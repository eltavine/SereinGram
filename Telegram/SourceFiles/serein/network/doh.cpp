#include "serein/hooks/network/doh.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"

namespace Serein::Network {

QStringList CustomDohHosts() {
	const auto host = ForDevice().Get(ServiceSettings::kCustomDoh);
	return host.isEmpty() ? QStringList() : QStringList{ host };
}

} // namespace Serein::Network
