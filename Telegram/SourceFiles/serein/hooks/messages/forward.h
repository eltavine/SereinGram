#pragma once

namespace Serein::Hooks {

[[nodiscard]] int RememberedForwardOptions();
void RememberForwardOptions(int options, bool shown = true);

template <typename Options>
[[nodiscard]] Options DefaultForwardOptions() {
	return static_cast<Options>(RememberedForwardOptions());
}

} // namespace Serein::Hooks
