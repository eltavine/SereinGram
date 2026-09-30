#include "serein/settings/privacy.h"

#include "serein/features/ghost/model/policy.h"
#include "serein/privacy/options.h"
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

void AddAccountToggle(
		SectionBuilder &builder,
		const Option<bool> &option,
		rpl::producer<QString> title,
		QString id,
		QStringList keywords) {
	const auto session = builder.session();
	const auto button = builder.addButton({
		.id = std::move(id),
		.title = std::move(title),
		.st = &st::settingsButtonNoIcon,
		.toggled = ForAccount(session).Value(option),
		.keywords = std::move(keywords),
	});
	if (button) {
		button->toggledChanges(
		) | rpl::on_next([=](bool value) {
			Expects(ForAccount(session).Set(option, value));
		}, button->lifetime());
	}
}

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
	AddAccountToggle(builder, Ghost::kGhostMode,
		tr::lng_serein_ghost_mode(),
		u"serein/privacy/ghost-mode"_q,
		{ u"ghost"_q, u"online"_q, u"typing"_q });
	AddAccountToggle(builder, Ghost::kGhostHideOnline,
		tr::lng_serein_ghost_hide_online(),
		u"serein/privacy/ghost-hide-online"_q,
		{ u"ghost"_q, u"online"_q });
	AddAccountToggle(builder, Ghost::kGhostHideTyping,
		tr::lng_serein_ghost_hide_typing(),
		u"serein/privacy/ghost-hide-typing"_q,
		{ u"ghost"_q, u"typing"_q });
	builder.addDividerText(tr::lng_serein_ghost_note());
	AddToggle(builder, Privacy::kDemoMode,
		tr::lng_serein_demo_mode(),
		u"serein/privacy/demo-mode"_q,
		{ u"presentation"_q, u"capture"_q });
	builder.addDividerText(tr::lng_serein_demo_mode_note());
	AddToggle(builder, Privacy::kHideReadTime,
		tr::lng_serein_hide_read_time(),
		u"serein/privacy/hide-read-time"_q,
		{ u"read"_q, u"time"_q });
	AddToggle(builder, Privacy::kHideSharePhonePrompt,
		tr::lng_serein_hide_share_phone_prompt(),
		u"serein/privacy/hide-share-phone-prompt"_q,
		{ u"share"_q, u"phone"_q });
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/privacy/profile-id-format"_q,
		.title = tr::lng_serein_profile_id_format(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Privacy::kProfileIdFormat)
			| rpl::map(ProfileIdFormatLabel),
		.onClick = [=] { controller->show(Box(ProfileIdFormatBox)); },
		.keywords = { u"profile"_q, u"ID"_q },
	});
	AddToggle(builder, Privacy::kShowProfileDc,
		tr::lng_serein_show_profile_dc(),
		u"serein/privacy/show-profile-dc"_q,
		{ u"profile"_q, u"DC"_q });
	AddToggle(builder, Privacy::kHideProfileGifts,
		tr::lng_serein_hide_profile_gifts(),
		u"serein/privacy/hide-profile-gifts"_q,
		{ u"profile"_q, u"gifts"_q });
	AddToggle(builder, Privacy::kHideCreateTodo,
		tr::lng_serein_hide_create_todo(),
		u"serein/privacy/hide-create-todo"_q,
		{ u"todo"_q, u"list"_q });
});

const SectionBuildMethod PrivacySection::kBuild = kMeta.build;

} // namespace

Settings::Type PrivacyId() {
	return PrivacySection::Id();
}

} // namespace Serein
