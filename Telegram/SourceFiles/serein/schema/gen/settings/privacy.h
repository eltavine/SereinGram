// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Privacy {

inline constexpr auto kDemoMode = Option<bool>{
	"serein.demoMode",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_demo_mode",
	0 };
inline constexpr auto kAutoDemoMode = Option<bool>{
	"serein.autoDemoMode",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_auto_demo_mode",
	0 };
inline constexpr auto kLockSettings = Option<bool>{
	"serein.lockSettings",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_lock_settings",
	0 };
inline constexpr auto kHideReadTime = Option<bool>{
	"serein.hideReadTime",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_hide_read_time",
	0 };
inline constexpr auto kHideSharePhonePrompt = Option<bool>{
	"serein.hideSharePhonePrompt",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_hide_share_phone_prompt",
	0 };
inline constexpr auto kProfileIdFormat = Option<int>{
	"serein.profileIdFormat",
	Scope::Device,
	0,
	Category::Privacy,
	"lng_serein_profile_id_format",
	0,
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 2));
	} };
inline constexpr auto kShowProfileDc = Option<bool>{
	"serein.showProfileDc",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_show_profile_dc",
	0 };
inline constexpr auto kShowRegistrationDate = Option<bool>{
	"serein.showRegistrationDate",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_show_registration_date",
	0 };
inline constexpr auto kShowSessionDetails = Option<bool>{
	"serein.showSessionDetails",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_show_session_details",
	0 };
inline constexpr auto kLocalNames = Option<bool>{
	"serein.localNames",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_local_names",
	0 };
inline constexpr auto kHideProfileGifts = Option<bool>{
	"serein.hideProfileGifts",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_hide_profile_gifts",
	0 };
inline constexpr auto kHideCreateTodo = Option<bool>{
	"serein.hideCreateTodo",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_hide_create_todo",
	0 };
inline constexpr auto kSaveProtectedContent = Option<bool>{
	"serein.saveProtectedContent",
	Scope::Device,
	false,
	Category::Privacy,
	"lng_serein_save_protected_content",
	0 };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kDemoMode));
	Expects(registry.Add(kAutoDemoMode));
	Expects(registry.Add(kLockSettings));
	Expects(registry.Add(kHideReadTime));
	Expects(registry.Add(kHideSharePhonePrompt));
	Expects(registry.Add(kProfileIdFormat));
	Expects(registry.Add(kShowProfileDc));
	Expects(registry.Add(kShowRegistrationDate));
	Expects(registry.Add(kShowSessionDetails));
	Expects(registry.Add(kLocalNames));
	Expects(registry.Add(kHideProfileGifts));
	Expects(registry.Add(kHideCreateTodo));
	Expects(registry.Add(kSaveProtectedContent));
}

} // namespace Serein::Privacy
