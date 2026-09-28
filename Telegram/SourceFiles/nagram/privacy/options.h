#pragma once

#include "nagram/core/options.h"

namespace Nagram::Privacy {

inline constexpr auto kDemoMode = Option<bool>{
	"nagram.demoMode", Scope::Device, false,
	Category::Privacy, "lng_nagram_demo_mode" };

[[nodiscard]] inline bool DemoMode() {
	return ForDevice().Get(kDemoMode);
}

inline constexpr auto kHideReadTime = Option<bool>{
	"nagram.hideReadTime", Scope::Device, false,
	Category::Privacy, "lng_nagram_hide_read_time" };
inline constexpr auto kHideSharePhonePrompt = Option<bool>{
	"nagram.hideSharePhonePrompt", Scope::Device, false,
	Category::Privacy, "lng_nagram_hide_share_phone_prompt" };
inline constexpr auto kProfileIdFormat = Option<int>{
	"nagram.profileIdFormat", Scope::Device, 0,
	Category::Privacy, "lng_nagram_profile_id_format", 0,
	[](const int &value) { return value >= 0 && value <= 2; } };
inline constexpr auto kShowProfileDc = Option<bool>{
	"nagram.showProfileDc", Scope::Device, false,
	Category::Privacy, "lng_nagram_show_profile_dc" };
inline constexpr auto kHideProfileGifts = Option<bool>{
	"nagram.hideProfileGifts", Scope::Device, false,
	Category::Privacy, "lng_nagram_hide_profile_gifts" };
inline constexpr auto kHideCreateTodo = Option<bool>{
	"nagram.hideCreateTodo", Scope::Device, false,
	Category::Privacy, "lng_nagram_hide_create_todo" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kDemoMode));
	Expects(registry.Add(kHideReadTime));
	Expects(registry.Add(kHideSharePhonePrompt));
	Expects(registry.Add(kProfileIdFormat));
	Expects(registry.Add(kShowProfileDc));
	Expects(registry.Add(kHideProfileGifts));
	Expects(registry.Add(kHideCreateTodo));
}

} // namespace Nagram::Privacy
