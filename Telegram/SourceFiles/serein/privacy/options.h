#pragma once

#include "serein/core/options.h"

namespace Serein::Privacy {

inline constexpr auto kDemoMode = Option<bool>{
	"serein.demoMode", Scope::Device, false,
	Category::Privacy, "lng_serein_demo_mode" };

[[nodiscard]] inline bool DemoMode() {
	return ForDevice().Get(kDemoMode);
}

inline constexpr auto kHideReadTime = Option<bool>{
	"serein.hideReadTime", Scope::Device, false,
	Category::Privacy, "lng_serein_hide_read_time" };
inline constexpr auto kHideSharePhonePrompt = Option<bool>{
	"serein.hideSharePhonePrompt", Scope::Device, false,
	Category::Privacy, "lng_serein_hide_share_phone_prompt" };
inline constexpr auto kProfileIdFormat = Option<int>{
	"serein.profileIdFormat", Scope::Device, 0,
	Category::Privacy, "lng_serein_profile_id_format", 0,
	[](const int &value) { return value >= 0 && value <= 2; } };
inline constexpr auto kShowProfileDc = Option<bool>{
	"serein.showProfileDc", Scope::Device, false,
	Category::Privacy, "lng_serein_show_profile_dc" };
inline constexpr auto kHideProfileGifts = Option<bool>{
	"serein.hideProfileGifts", Scope::Device, false,
	Category::Privacy, "lng_serein_hide_profile_gifts" };
inline constexpr auto kHideCreateTodo = Option<bool>{
	"serein.hideCreateTodo", Scope::Device, false,
	Category::Privacy, "lng_serein_hide_create_todo" };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kDemoMode));
	Expects(registry.Add(kHideReadTime));
	Expects(registry.Add(kHideSharePhonePrompt));
	Expects(registry.Add(kProfileIdFormat));
	Expects(registry.Add(kShowProfileDc));
	Expects(registry.Add(kHideProfileGifts));
	Expects(registry.Add(kHideCreateTodo));
}

} // namespace Serein::Privacy
