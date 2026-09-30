// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Privacy {

inline const auto kToggleRows = std::array<ToggleRow, 9>{ {
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
	AddToggle(builder, kToggleRows[3]);
	AddChoice(builder, {
		.option = &kProfileIdFormat,
		.title = tr::lng_serein_profile_id_format,
		.id = u"serein/privacy/profile-id-format"_q,
		.keywords = { u"profile"_q, u"ID"_q },
		.values = { 0, 1, 2 },
		.labels = { tr::lng_serein_id_off, tr::lng_serein_id_bot_api, tr::lng_serein_id_raw },
	});
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddNote(builder, tr::lng_serein_registration_date_note);
	AddToggle(builder, kToggleRows[6]);
	AddToggle(builder, kToggleRows[7]);
	AddToggle(builder, kToggleRows[8]);
	AddNote(builder, tr::lng_serein_save_protected_content_note);
}

} // namespace Serein::Privacy
