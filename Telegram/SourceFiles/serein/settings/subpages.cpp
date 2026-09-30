#include "serein/settings/subpages.h"

#include "serein/features/history/viewer.h"
#include "serein/settings/gen/ghost_rows.h"
#include "serein/settings/gen/history_rows.h"
#include "serein/settings/privacy.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class GhostSection final : public Section<GhostSection> {
public:
	GhostSection(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

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
	Ghost::AddLayout(builder);
});

const SectionBuildMethod GhostSection::kBuild = kGhostMeta.build;

class HistorySection final : public Section<HistorySection> {
public:
	HistorySection(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

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
	builder.addButton({
		.id = u"serein/privacy/history-clear-all"_q,
		.title = tr::lng_serein_history_clear_all(),
		.st = &st::settingsAttentionButton,
		.onClick = [=] {
			if (controller) {
				HistoryFeature::ConfirmClearHistory(controller, nullptr);
			}
		},
		.keywords = { u"history"_q, u"deleted"_q, u"clear"_q },
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
