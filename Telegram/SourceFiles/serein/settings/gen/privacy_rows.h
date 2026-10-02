// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Privacy {

inline const auto kToggleRows = std::array<ToggleRow, 13>{ {
	{
		&kDemoMode,
		tr::lng_serein_demo_mode,
		u"serein/privacy/demo-mode"_q,
		{ u"presentation"_q, u"capture"_q },
	},
	{
		&kAutoDemoMode,
		tr::lng_serein_auto_demo_mode,
		u"serein/privacy/auto-demo-mode"_q,
		{ u"presentation"_q, u"OBS"_q, u"recording"_q, u"streaming"_q },
	},
	{
		&kLockSettings,
		tr::lng_serein_lock_settings,
		u"serein/privacy/lock-settings"_q,
		{ u"lock"_q, u"passcode"_q, u"settings"_q },
	},
	{
		&kHideReadTime,
		tr::lng_serein_hide_read_time,
		u"serein/privacy/hide-read-time"_q,
		{ u"read"_q, u"time"_q },
	},
	{
		&kHideSharePhonePrompt,
		tr::lng_serein_hide_share_phone_prompt,
		u"serein/privacy/hide-share-phone-prompt"_q,
		{ u"share"_q, u"phone"_q },
	},
	{
		&kShowProfileDc,
		tr::lng_serein_show_profile_dc,
		u"serein/privacy/show-profile-dc"_q,
		{ u"profile"_q, u"DC"_q },
	},
	{
		&kShowRegistrationDate,
		tr::lng_serein_show_registration_date,
		u"serein/privacy/show-registration-date"_q,
		{ u"profile"_q, u"registration"_q, u"account age"_q },
	},
	{
		&kShowContactStatus,
		tr::lng_serein_show_contact_status,
		u"serein/privacy/show-contact-status"_q,
		{ u"profile"_q, u"contact"_q, u"mutual contact"_q },
	},
	{
		&kShowSessionDetails,
		tr::lng_serein_show_session_details,
		u"serein/privacy/show-session-details"_q,
		{ u"sessions"_q, u"devices"_q, u"API ID"_q, u"login"_q },
	},
	{
		&kLocalNames,
		tr::lng_serein_local_names,
		u"serein/privacy/local-names"_q,
		{ u"local name"_q, u"alias"_q, u"rename"_q, u"nickname"_q },
	},
	{
		&kHideProfileGifts,
		tr::lng_serein_hide_profile_gifts,
		u"serein/privacy/hide-profile-gifts"_q,
		{ u"profile"_q, u"gifts"_q },
	},
	{
		&kHideCreateTodo,
		tr::lng_serein_hide_create_todo,
		u"serein/privacy/hide-create-todo"_q,
		{ u"todo"_q, u"list"_q },
	},
	{
		&kSaveProtectedContent,
		tr::lng_serein_save_protected_content,
		u"serein/privacy/save-protected-content"_q,
		{ u"protected"_q, u"save"_q, u"copy"_q, u"noforwards"_q },
	},
} };

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder) {
	AddToggle(builder, kToggleRows[0]);
	AddNote(builder, tr::lng_serein_demo_mode_note);
	AddToggle(builder, kToggleRows[1]);
	AddNote(builder, tr::lng_serein_auto_demo_mode_note);
	AddToggle(builder, kToggleRows[2]);
	AddNote(builder, tr::lng_serein_lock_settings_note);
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddChoice(builder, {
		.option = &kProfileIdFormat,
		.title = tr::lng_serein_profile_id_format,
		.id = u"serein/privacy/profile-id-format"_q,
		.keywords = { u"profile"_q, u"ID"_q },
		.values = { 0, 1, 2 },
		.labels = { tr::lng_serein_id_off, tr::lng_serein_id_bot_api, tr::lng_serein_id_raw },
	});
	AddToggle(builder, kToggleRows[5]);
	AddToggle(builder, kToggleRows[6]);
	AddNote(builder, tr::lng_serein_registration_date_note);
	AddToggle(builder, kToggleRows[7]);
	AddNote(builder, tr::lng_serein_contact_status_note);
	AddToggle(builder, kToggleRows[8]);
	AddNote(builder, tr::lng_serein_session_details_note);
	AddToggle(builder, kToggleRows[9]);
	AddNote(builder, tr::lng_serein_local_names_note);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	AddToggle(builder, kToggleRows[12]);
	AddNote(builder, tr::lng_serein_save_protected_content_note);
}

} // namespace Serein::Privacy
