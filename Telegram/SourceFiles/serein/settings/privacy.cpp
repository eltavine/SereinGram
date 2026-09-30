#include "serein/settings/privacy.h"

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

QString ProfileIdFormatLabel(int format) {
	return (format == 1)
		? tr::lng_serein_id_bot_api(tr::now)
		: (format == 2)
		? tr::lng_serein_id_raw(tr::now)
		: tr::lng_serein_id_off(tr::now);
}

void ProfileIdFormatBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_profile_id_format());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		ForDevice().Get(Privacy::kProfileIdFormat));
	for (auto value = 0; value != 3; ++value) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value, ProfileIdFormatLabel(value),
			st::settingsSendType), st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(ForDevice().Set(Privacy::kProfileIdFormat, value));
		box->closeBox();
	});
}

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
	Privacy::AddLayout(builder, {
		.profileIdFormat = [&] {
			builder.addButton({
				.id = u"serein/privacy/profile-id-format"_q,
				.title = tr::lng_serein_profile_id_format(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Privacy::kProfileIdFormat)
					| rpl::map(ProfileIdFormatLabel),
				.onClick = [=] { controller->show(Box(ProfileIdFormatBox)); },
				.keywords = { u"profile"_q, u"ID"_q },
			});
		},
	});
});

const SectionBuildMethod PrivacySection::kBuild = kMeta.build;

} // namespace

Settings::Type PrivacyId() {
	return PrivacySection::Id();
}

} // namespace Serein
