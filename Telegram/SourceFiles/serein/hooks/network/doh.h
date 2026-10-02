#pragma once

#include <QtCore/QStringList>

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

} // namespace Serein::Network
