#include "serein/settings/home.h"
#include "serein/settings/interface.h"
#include "serein/settings/licenses.h"
#include "serein/settings/messages.h"
#include "serein/settings/menu.h"
#include "serein/settings/chats.h"
#include "serein/settings/compose.h"
#include "serein/settings/config.h"
#include "serein/settings/media.h"
#include "serein/settings/privacy.h"
#include "serein/settings/services.h"
#include "serein/settings/presets.h"
#include "serein/settings/rules.h"
#include "serein/settings/subpages.h"
#include "serein/settings/page.h"
#include "serein/settings/gen/ghost_rows.h"
#include "serein/settings/gen/history_rows.h"
#include "serein/settings/gen/chats_rows.h"
#include "serein/settings/gen/compose_rows.h"
#include "serein/settings/gen/filters_rows.h"
#include "serein/settings/gen/interface_rows.h"
#include "serein/settings/gen/media_rows.h"
#include "serein/settings/gen/menu_rows.h"
#include "serein/settings/gen/messages_rows.h"
#include "serein/settings/gen/privacy_rows.h"
#include "serein/settings/gen/services_rows.h"
#include "serein/settings/home_cover.h"
#include "serein/features/updates/checker.h"

#include "boxes/about_box.h"
#include "core/click_handler_types.h"
#include "lang/lang_keys.h"
#include "settings/sections/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class Home final : public Page<Home> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_settings();
	}

	static const SectionBuildMethod kBuild;

};

[[nodiscard]] rpl::producer<QString> StateLabel(rpl::producer<bool> on) {
	return rpl::conditional(
		std::move(on),
		tr::lng_serein_config_on(),
		tr::lng_serein_config_off());
}

void AddFeatured(SectionBuilder &builder) {
	using namespace rpl::mappers;
	const auto controller = builder.controller();
	auto &account = ForAccount(builder.session());
	builder.addButton({
		.id = u"serein/home/ghost"_q,
		.title = (*Ghost::kSubpageTitle)(),
		.icon = { Ghost::kSubpageIcon },
		.label = StateLabel(rpl::combine(
			account.Value(Ghost::kGhostMode),
			ForDevice().Value(Ghost::kGhostAllAccounts)) | rpl::map(_1 || _2)),
		.onClick = [=] {
			if (controller) {
				controller->showSettings(GhostId());
			}
		},
		.keywords = { u"ghost"_q, u"stealth"_q, u"online"_q, u"read"_q },
	});
	builder.addButton({
		.id = u"serein/home/history"_q,
		.title = (*HistorySettings::kSubpageTitle)(),
		.icon = { HistorySettings::kSubpageIcon },
		.label = StateLabel(rpl::combine(
			account.Value(HistorySettings::kHistorySaveDeleted),
			account.Value(HistorySettings::kHistorySaveEdits)
		) | rpl::map(_1 || _2)),
		.onClick = [=] {
			if (controller) {
				controller->showSettings(HistoryId());
			}
		},
		.keywords = { u"deleted"_q, u"edited"_q, u"history"_q },
	});
	builder.addButton({
		.id = u"serein/home/presets"_q,
		.title = tr::lng_serein_presets(),
		.icon = { &st::menuIconCustomize },
		.onClick = [=] {
			if (controller) {
				Presets::ShowPresets(controller);
			}
		},
		.keywords = { u"preset"_q, u"setup"_q, u"quick"_q },
	});
	if (controller) {
		crl::on_main(controller, [=] { Presets::OfferPresetsOnce(controller); });
	}
}

void AddGroup(
		SectionBuilder &builder,
		rpl::producer<QString> title,
		FnMut<void()> fill) {
	builder.addDivider();
	builder.addSkip();
	builder.addSubsectionTitle(std::move(title));
	fill();
	builder.addSkip();
}

void AddUpdateCheck(SectionBuilder &builder) {
	if (!Updates::UpdateChecksAvailable()) {
		return;
	}
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/home/check-updates"_q,
		.title = tr::lng_serein_update_check_now(),
		.icon = { &st::menuIconDownload },
		.label = Updates::StateValue(
		) | rpl::map([](const Updates::UpdateState &state) {
			return (state.status == Updates::Status::Checking)
				? tr::lng_serein_update_checking(tr::now)
				: QString();
		}),
		.onClick = [=] {
			Updates::CheckForUpdatesNow(crl::guard(controller, [=](
					Updates::CheckResult result) {
				if (result == Updates::CheckResult::UpToDate) {
					controller->showToast(tr::lng_serein_update_latest(tr::now));
				} else if (result == Updates::CheckResult::Failed) {
					controller->showToast(tr::lng_serein_update_failed(tr::now));
				}
			}));
		},
		.keywords = { u"update"_q, u"version"_q, u"GitHub"_q },
	});
}

void AddAbout(SectionBuilder &builder) {
	builder.addDivider();
	builder.addSkip();
	builder.addSubsectionTitle(tr::lng_serein_home_about());
	AddUpdateCheck(builder);
	builder.addButton({
		.title = tr::lng_serein_source_code(),
		.icon = { &st::menuIconLink },
		.onClick = [] {
			UrlClickHandler::Open(u"https://github.com/eltavine/SereinGram"_q);
		},
	});
	const auto controller = builder.controller();
	builder.addButton({
		.title = tr::lng_serein_licenses(),
		.icon = { &st::menuIconFile },
		.onClick = [=] {
			if (controller) {
				ShowLicenses(controller);
			}
		},
	});
	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = Home::Id(),
	.parentId = MainId(),
	.title = &tr::lng_serein_settings,
	.icon = &st::menuIconSerein,
}, [](SectionBuilder &builder) {
	AddHomeCover(builder);
	builder.addDivider();
	builder.addSkip();
	AddFeatured(builder);
	builder.addSkip();
	AddGroup(builder, tr::lng_serein_home_appearance(), [&] {
		Interface::AddSubpageButton(builder, InterfaceId());
		Messages::AddSubpageButton(builder, MessagesId());
		Chats::AddSubpageButton(builder, ChatsId());
		Media::AddSubpageButton(builder, MediaId());
		Menu::AddSubpageButton(builder, MenuId());
	});
	AddGroup(builder, tr::lng_serein_home_tools(), [&] {
		Compose::AddSubpageButton(builder, ComposeId());
		ServiceSettings::AddSubpageButton(builder, ServicesId());
		Filters::AddSubpageButton(builder, RulesId());
	});
	AddGroup(builder, tr::lng_serein_home_privacy(), [&] {
		Privacy::AddSubpageButton(builder, PrivacyId());
		AddConfigButton(builder);
	});
	AddAbout(builder);
	builder.addDividerText(tr::lng_serein_settings_note());
});

const SectionBuildMethod Home::kBuild = kMeta.build;

} // namespace

Settings::Type HomeId() {
	return Home::Id();
}

void AddSettingsEntry(SectionBuilder &builder) {
	builder.addSectionButton({
		.title = tr::lng_serein_settings(),
		.targetSection = Home::Id(),
		.icon = { &st::menuIconSerein },
		.keywords = { u"Serein"_q, u"desktop"_q },
	});
}

} // namespace Serein
