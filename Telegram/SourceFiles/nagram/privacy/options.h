#pragma once

#include "nagram/core/options.h"

namespace Nagram::Privacy {

inline constexpr auto kHideReadTime = Option<bool>{
	"nagram.hideReadTime", Scope::Device, false,
	Category::Privacy, "lng_nagram_hide_read_time" };
inline constexpr auto kHideSharePhonePrompt = Option<bool>{
	"nagram.hideSharePhonePrompt", Scope::Device, false,
	Category::Privacy, "lng_nagram_hide_share_phone_prompt" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kHideReadTime));
	Expects(registry.Add(kHideSharePhonePrompt));
}

} // namespace Nagram::Privacy
