#include "serein/hooks/messages/forward.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/compose.h"

namespace Serein {

int Hooks::RememberedForwardOptions() {
	auto &options = ForDevice();
	return options.Get(Compose::kRememberForwardOptions)
		? options.Get(Compose::kLastForwardOptions)
		: 0;
}

void Hooks::RememberForwardOptions(int value, bool shown) {
	auto &options = ForDevice();
	if (shown && options.Get(Compose::kRememberForwardOptions)) {
		Expects(options.Set(Compose::kLastForwardOptions, value));
	}
}

} // namespace Serein
