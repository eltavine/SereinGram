// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/menu.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"

#include <array>

namespace Serein::Menu {

inline const auto kToggleRows = std::array<ToggleRow, 1>{ {
	{
		&kConfirmRepeat,
		tr::lng_serein_menu_confirm_repeat,
		u"serein/menu/confirm-repeat"_q,
		{ u"repeat"_q, u"confirm"_q },
	},
} };

struct CustomRows {
	CustomRow messageMenu;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddSection(builder, {
		u"serein/menu/items"_q,
		tr::lng_serein_menu_items,
		{ u"menu"_q, u"action"_q },
	});
	custom.messageMenu();
	EndSection(builder, tr::lng_serein_menu_note);
	AddSection(builder, {
		u"serein/menu/repeat-rating"_q,
		tr::lng_serein_section_repeat_rating,
		{ u"repeat"_q, u"rating"_q, u"reply"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddText(builder, {
		.option = &kQuickRatingFirst,
		.title = tr::lng_serein_quick_rating_first,
		.id = u"serein/menu/quick-rating-first"_q,
		.keywords = { u"rating"_q, u"reply"_q, u"great"_q },
		.placeholder = tr::lng_serein_quick_rating_unset,
	});
	AddText(builder, {
		.option = &kQuickRatingSecond,
		.title = tr::lng_serein_quick_rating_second,
		.id = u"serein/menu/quick-rating-second"_q,
		.keywords = { u"rating"_q, u"reply"_q, u"poor"_q },
		.placeholder = tr::lng_serein_quick_rating_unset,
	});
	EndSection(builder, tr::lng_serein_quick_rating_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_menu;
inline const auto kSubpageIcon = &st::menuIconReorder;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	builder.addSectionButton({
		.title = (*kSubpageTitle)(),
		.targetSection = section,
		.icon = { kSubpageIcon },
		.keywords = { u"menu"_q, u"actions"_q },
	});
}

} // namespace Serein::Menu
