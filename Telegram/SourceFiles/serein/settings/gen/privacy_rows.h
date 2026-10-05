// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::Privacy {

inline const auto kToggleRows = std::array<ToggleRow, 13>{ {
	{
		.option = &kDemoMode,
		.title = tr::lng_serein_demo_mode,
		.id = u"serein/privacy/demo-mode"_q,
		.keywords = { u"presentation"_q, u"capture"_q },
		.icon = &st::menuIconSpoiler,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_demo_mode_about,
	},
	{
		.option = &kAutoDemoMode,
		.title = tr::lng_serein_auto_demo_mode,
		.id = u"serein/privacy/auto-demo-mode"_q,
		.keywords = { u"presentation"_q, u"OBS"_q, u"recording"_q, u"streaming"_q },
		.icon = &st::menuIconStartStreamWith,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_auto_demo_mode_about,
	},
	{
		.option = &kLockSettings,
		.title = tr::lng_serein_lock_settings,
		.id = u"serein/privacy/lock-settings"_q,
		.keywords = { u"lock"_q, u"passcode"_q, u"settings"_q },
		.icon = &st::menuIcon2SV,
		.tile = &st::settingsIconBg4,
		.about = tr::lng_serein_lock_settings_about,
	},
	{
		.option = &kShowProfileDc,
		.title = tr::lng_serein_show_profile_dc,
		.id = u"serein/privacy/show-profile-dc"_q,
		.keywords = { u"profile"_q, u"DC"_q },
		.icon = &st::menuIconStorage,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_show_profile_dc_about,
	},
	{
		.option = &kShowRegistrationDate,
		.title = tr::lng_serein_show_registration_date,
		.id = u"serein/privacy/show-registration-date"_q,
		.keywords = { u"profile"_q, u"registration"_q, u"account age"_q },
		.icon = &st::menuIconSchedule,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_show_registration_date_about,
	},
	{
		.option = &kShowContactStatus,
		.title = tr::lng_serein_show_contact_status,
		.id = u"serein/privacy/show-contact-status"_q,
		.keywords = { u"profile"_q, u"contact"_q, u"mutual contact"_q },
		.icon = &st::menuIconInvite,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_show_contact_status_about,
	},
	{
		.option = &kShowSessionDetails,
		.title = tr::lng_serein_show_session_details,
		.id = u"serein/privacy/show-session-details"_q,
		.keywords = { u"sessions"_q, u"devices"_q, u"API ID"_q, u"login"_q },
		.icon = &st::menuIconDevices,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_show_session_details_about,
	},
	{
		.option = &kLocalNames,
		.title = tr::lng_serein_local_names,
		.id = u"serein/privacy/local-names"_q,
		.keywords = { u"local name"_q, u"alias"_q, u"rename"_q, u"nickname"_q },
		.icon = &st::menuIconTagRename,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_local_names_about,
	},
	{
		.option = &kHideProfileGifts,
		.title = tr::lng_serein_hide_profile_gifts,
		.id = u"serein/privacy/hide-profile-gifts"_q,
		.keywords = { u"profile"_q, u"gifts"_q },
		.icon = &st::menuIconGiftPremium,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_hide_profile_gifts_about,
	},
	{
		.option = &kHideReadTime,
		.title = tr::lng_serein_hide_read_time,
		.id = u"serein/privacy/hide-read-time"_q,
		.keywords = { u"read"_q, u"time"_q },
		.icon = &st::menuIconMarkRead,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_read_time_about,
	},
	{
		.option = &kHideSharePhonePrompt,
		.title = tr::lng_serein_hide_share_phone_prompt,
		.id = u"serein/privacy/hide-share-phone-prompt"_q,
		.keywords = { u"share"_q, u"phone"_q },
		.icon = &st::menuIconPhone,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_share_phone_prompt_about,
	},
	{
		.option = &kHideCreateTodo,
		.title = tr::lng_serein_hide_create_todo,
		.id = u"serein/privacy/hide-create-todo"_q,
		.keywords = { u"todo"_q, u"list"_q },
		.icon = &st::menuIconCreateTodoList,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_hide_create_todo_about,
	},
	{
		.option = &kSaveProtectedContent,
		.title = tr::lng_serein_save_protected_content,
		.id = u"serein/privacy/save-protected-content"_q,
		.keywords = { u"protected"_q, u"save"_q, u"copy"_q, u"noforwards"_q },
		.icon = &st::menuIconSaveImage,
		.tile = &st::settingsIconBg5,
		.about = tr::lng_serein_save_protected_content_about,
	},
} };

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder) {
	AddSection(builder, {
		u"serein/privacy/screen"_q,
		tr::lng_serein_section_screen,
		{ u"streamer"_q, u"screen"_q, u"lock"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddNote(builder, tr::lng_serein_demo_mode_note);
	AddToggle(builder, kToggleRows[1]);
	AddNote(builder, tr::lng_serein_auto_demo_mode_note);
	AddToggle(builder, kToggleRows[2]);
	EndSection(builder, tr::lng_serein_lock_settings_note);
	AddSection(builder, {
		u"serein/privacy/profiles"_q,
		tr::lng_serein_section_profiles,
		{ u"profile"_q, u"ID"_q },
	});
	AddChoice(builder, {
		.option = &kProfileIdFormat,
		.title = tr::lng_serein_profile_id_format,
		.id = u"serein/privacy/profile-id-format"_q,
		.keywords = { u"profile"_q, u"ID"_q },
		.values = { 0, 1, 2 },
		.labels = { tr::lng_serein_id_off, tr::lng_serein_id_bot_api, tr::lng_serein_id_raw },
		.icon = &st::menuIconPersonal,
		.tile = &st::settingsIconBg2,
		.about = tr::lng_serein_profile_id_format_about,
	});
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddNote(builder, tr::lng_serein_registration_date_note);
	AddToggle(builder, kToggleRows[5]);
	AddNote(builder, tr::lng_serein_contact_status_note);
	AddToggle(builder, kToggleRows[6]);
	AddNote(builder, tr::lng_serein_session_details_note);
	AddToggle(builder, kToggleRows[7]);
	AddNote(builder, tr::lng_serein_local_names_note);
	AddToggle(builder, kToggleRows[8]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/privacy/prompts"_q,
		tr::lng_serein_section_prompts,
		{ u"prompt"_q, u"protected"_q },
	});
	AddToggle(builder, kToggleRows[9]);
	AddToggle(builder, kToggleRows[10]);
	AddToggle(builder, kToggleRows[11]);
	AddToggle(builder, kToggleRows[12]);
	EndSection(builder, tr::lng_serein_save_protected_content_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_privacy;
inline const auto kSubpageIcon = &st::menuIconLock;
inline const auto kSubpageTile = &st::settingsIconBg4;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"privacy"_q, u"phone"_q },
	});
}

} // namespace Serein::Privacy
