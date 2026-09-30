#pragma once

#include "serein/compose/options.h"

namespace Serein::Compose {

inline bool Hidden(const Option<bool> &option) {
	return ForDevice().Get(option);
}

inline rpl::producer<> ButtonsChanged() {
	return ForDevice().changes()
		| rpl::filter([](std::string_view key) {
			return RegisteredOptions().HasFlag(
				key, Flag::RefreshComposeButtons);
		})
		| rpl::to_empty;
}

} // namespace Serein::Compose
