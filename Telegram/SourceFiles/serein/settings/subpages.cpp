#include "serein/settings/subpages.h"

#include "serein/features/history/viewer.h"
#include "serein/settings/ghost_exceptions.h"
#include "serein/settings/gen/ghost_rows.h"
#include "serein/settings/gen/history_rows.h"
#include "serein/settings/privacy.h"
#include "serein/settings/page.h"
#include "serein/settings/rows.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class GhostSection final : public Page<GhostSection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return (*Ghost::kSubpageTitle)();
	}

	static const SectionBuildMethod kBuild;

};

const auto kGhostMeta = BuildHelper({
	.id = GhostSection::Id(),
	.parentId = PrivacyId(),
	.title = Ghost::kSubpageTitle,
	.icon = Ghost::kSubpageIcon,
}, [](SectionBuilder &builder) {
	Ghost::AddLayout(builder, {
		.readReceiptExceptions = [&] { AddReadExceptionsRow(builder); },
	});
});

const SectionBuildMethod GhostSection::kBuild = kGhostMeta.build;

class HistorySection final : public Page<HistorySection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return (*HistorySettings::kSubpageTitle)();
	}

	static const SectionBuildMethod kBuild;

};

const auto kHistoryMeta = BuildHelper({
	.id = HistorySection::Id(),
	.parentId = PrivacyId(),
	.title = HistorySettings::kSubpageTitle,
	.icon = HistorySettings::kSubpageIcon,
}, [](SectionBuilder &builder) {
	HistorySettings::AddLayout(builder);
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = u"serein/privacy/history-saved-chats"_q,
		.title = tr::lng_serein_history_saved_chats(),
		.onClick = [=] {
			if (controller) {
				HistoryFeature::ShowSavedChats(controller);
			}
		},
		.keywords = { u"history"_q, u"deleted"_q, u"chats"_q },
		.visual = {
			.icon = &st::menuIconChats,
			.about = tr::lng_serein_history_saved_chats_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/privacy/history-clear-all"_q,
		.title = tr::lng_serein_history_clear_all(),
		.onClick = [=] {
			if (controller) {
				HistoryFeature::ConfirmClearHistory(controller, nullptr);
			}
		},
		.keywords = { u"history"_q, u"deleted"_q, u"clear"_q },
		.visual = {
			.icon = &st::menuIconClear,
			.about = tr::lng_serein_history_clear_all_about,
		},
		.st = &st::sereinSettingsAttentionButtonDescribed,
	});
});

const SectionBuildMethod HistorySection::kBuild = kHistoryMeta.build;

} // namespace

Settings::Type GhostId() {
	return GhostSection::Id();
}

Settings::Type HistoryId() {
	return HistorySection::Id();
}

} // namespace Serein
