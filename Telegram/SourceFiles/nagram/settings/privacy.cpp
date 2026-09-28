#include "nagram/settings/privacy.h"

#include "nagram/privacy/options.h"
#include "nagram/settings/home.h"
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

namespace Nagram {
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
		return tr::lng_nagram_privacy();
	}

	static const SectionBuildMethod kBuild;
};

void AddToggle(
		SectionBuilder &builder,
		const Option<bool> &option,
		rpl::producer<QString> title,
		QString id,
		QStringList keywords) {
	const auto button = builder.addButton({
		.id = std::move(id),
		.title = std::move(title),
		.st = &st::settingsButtonNoIcon,
		.toggled = ForDevice().Value(option),
		.keywords = std::move(keywords),
	});
	if (button) {
		button->toggledChanges(
		) | rpl::on_next([option](bool value) {
			Expects(ForDevice().Set(option, value));
		}, button->lifetime());
	}
}

QString ProfileIdFormatLabel(int format) {
	return (format == 1)
		? tr::lng_nagram_id_bot_api(tr::now)
		: (format == 2)
		? tr::lng_nagram_id_raw(tr::now)
		: tr::lng_nagram_id_off(tr::now);
}

void ProfileIdFormatBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_nagram_profile_id_format());
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
	.title = &tr::lng_nagram_privacy,
	.icon = &st::menuIconLock,
}, [](SectionBuilder &builder) {
	const auto session = builder.session();
	const auto button = builder.addButton({
		.id = u"nagram/privacy/hide-my-phone"_q,
		.title = tr::lng_nagram_hide_my_phone(),
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
	AddToggle(builder, Privacy::kDemoMode,
		tr::lng_nagram_demo_mode(),
		u"nagram/privacy/demo-mode"_q,
		{ u"presentation"_q, u"capture"_q });
	builder.addDividerText(tr::lng_nagram_demo_mode_note());
	AddToggle(builder, Privacy::kHideReadTime,
		tr::lng_nagram_hide_read_time(),
		u"nagram/privacy/hide-read-time"_q,
		{ u"read"_q, u"time"_q });
	AddToggle(builder, Privacy::kHideSharePhonePrompt,
		tr::lng_nagram_hide_share_phone_prompt(),
		u"nagram/privacy/hide-share-phone-prompt"_q,
		{ u"share"_q, u"phone"_q });
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"nagram/privacy/profile-id-format"_q,
		.title = tr::lng_nagram_profile_id_format(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Privacy::kProfileIdFormat)
			| rpl::map(ProfileIdFormatLabel),
		.onClick = [=] { controller->show(Box(ProfileIdFormatBox)); },
		.keywords = { u"profile"_q, u"ID"_q },
	});
	AddToggle(builder, Privacy::kShowProfileDc,
		tr::lng_nagram_show_profile_dc(),
		u"nagram/privacy/show-profile-dc"_q,
		{ u"profile"_q, u"DC"_q });
	AddToggle(builder, Privacy::kHideProfileGifts,
		tr::lng_nagram_hide_profile_gifts(),
		u"nagram/privacy/hide-profile-gifts"_q,
		{ u"profile"_q, u"gifts"_q });
	AddToggle(builder, Privacy::kHideCreateTodo,
		tr::lng_nagram_hide_create_todo(),
		u"nagram/privacy/hide-create-todo"_q,
		{ u"todo"_q, u"list"_q });
});

const SectionBuildMethod PrivacySection::kBuild = kMeta.build;

} // namespace

Settings::Type PrivacyId() {
	return PrivacySection::Id();
}

} // namespace Nagram
