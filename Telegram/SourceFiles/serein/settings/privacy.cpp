#include "serein/settings/privacy.h"

#include "serein/features/history/viewer.h"
#include "serein/privacy/options.h"
#include "serein/settings/gen/ghost_rows.h"
#include "serein/settings/gen/history_rows.h"
#include "serein/settings/gen/privacy_rows.h"
#include "serein/settings/home.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/main_session_settings.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/layers/generic_box.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class PrivacySection final : public Section<PrivacySection> {
public:
	PrivacySection(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_privacy();
	}

	static const SectionBuildMethod kBuild;
};

const auto kMeta = BuildHelper({
	.id = PrivacySection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_privacy,
	.icon = &st::menuIconLock,
}, [](SectionBuilder &builder) {
	const auto session = builder.session();
	const auto button = builder.addButton({
		.id = u"serein/privacy/hide-my-phone"_q,
		.title = tr::lng_serein_hide_my_phone(),
		.st = &st::settingsButtonNoIcon,
		.toggled = session->settings().phoneNumberHiddenValue(),
		.keywords = { u"phone"_q, u"number"_q },
	});
	if (button) {
		button->toggledChanges(
		) | rpl::on_next([=](bool value) {
			session->settings().setPhoneNumberHidden(value);
			session->saveSettingsDelayed();
		}, button->lifetime());
	}
	Ghost::AddLayout(builder);
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
	Privacy::AddLayout(builder);
});

const SectionBuildMethod PrivacySection::kBuild = kMeta.build;

} // namespace

Settings::Type PrivacyId() {
	return PrivacySection::Id();
}

} // namespace Serein
