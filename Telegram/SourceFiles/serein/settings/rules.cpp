#include "serein/settings/rules.h"

#include "serein/settings/home.h"
#include "serein/settings/gen/filters_rows.h"
#include "serein/settings/keyword_alerts.h"
#include "serein/filters/settings.h"
#include "serein/links/settings.h"
#include "serein/settings/page.h"
#include "serein/settings/rows.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class RulesSection final : public Page<RulesSection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_rules();
	}

	static const SectionBuildMethod kBuild;

};

const auto kMeta = BuildHelper({
	.id = RulesSection::Id(),
	.parentId = HomeId(),
	.title = Filters::kSubpageTitle,
	.icon = Filters::kSubpageIcon,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	builder.addSkip();
	AddRow(builder, {
		.id = u"serein/rules/messages"_q,
		.title = tr::lng_serein_filters(),
		.onClick = [=] {
			controller->show(Box(Filters::SettingsBox, &controller->session()));
		},
		.keywords = { u"filter"_q, u"regex"_q },
		.visual = {
			.icon = &st::menuIconTagFilter,
			.about = tr::lng_serein_filters_row_about,
		},
	});
	EndSection(builder, tr::lng_serein_rules_filters_about);
	builder.addSkip();
	AddRow(builder, {
		.id = u"serein/rules/links"_q,
		.title = tr::lng_serein_link_rules(),
		.onClick = [=] {
			controller->show(Box(Links::SettingsBox));
		},
		.keywords = { u"link"_q, u"URL"_q },
		.visual = {
			.icon = &st::menuIconLinks,
			.about = tr::lng_serein_link_rules_row_about,
		},
	});
	EndSection(builder, tr::lng_serein_rules_links_about);
	builder.addSkip();
	Filters::AddKeywordAlerts(builder);
	EndSection(builder, tr::lng_serein_rules_alerts_about);
});

const SectionBuildMethod RulesSection::kBuild = kMeta.build;

} // namespace

Settings::Type RulesId() {
	return RulesSection::Id();
}

} // namespace Serein
