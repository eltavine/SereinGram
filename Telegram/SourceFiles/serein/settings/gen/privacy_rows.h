// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::Privacy {

inline const auto kToggleRows = std::array<ToggleRow, 6>{ {
	{
		&kDemoMode,
		tr::lng_serein_demo_mode,
		u"serein/privacy/demo-mode"_q,
		{  },
	},
	{
		&kHideReadTime,
		tr::lng_serein_hide_read_time,
		u"serein/privacy/hide-read-time"_q,
		{  },
	},
	{
		&kHideSharePhonePrompt,
		tr::lng_serein_hide_share_phone_prompt,
		u"serein/privacy/hide-share-phone-prompt"_q,
		{  },
	},
	{
		&kShowProfileDc,
		tr::lng_serein_show_profile_dc,
		u"serein/privacy/show-profile-dc"_q,
		{  },
	},
	{
		&kHideProfileGifts,
		tr::lng_serein_hide_profile_gifts,
		u"serein/privacy/hide-profile-gifts"_q,
		{  },
	},
	{
		&kHideCreateTodo,
		tr::lng_serein_hide_create_todo,
		u"serein/privacy/hide-create-todo"_q,
		{  },
	},
} };

} // namespace Serein::Privacy
