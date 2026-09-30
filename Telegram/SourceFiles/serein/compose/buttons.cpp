#include "serein/hooks/compose/buttons.h"

#include "serein/core/options.h"

namespace Serein::Compose {

rpl::producer<> ButtonsChanged() {
	return ForDevice().changes()
		| rpl::filter([](std::string_view key) {
			return RegisteredOptions().HasFlag(
				key, Flag::RefreshComposeButtons);
		})
		| rpl::to_empty;
}

} // namespace Serein::Compose
