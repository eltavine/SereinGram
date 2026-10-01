#include "serein/settings/interface.h"

#include "serein/interface/options.h"
#include "serein/hooks/interface/main_menu.h"
#include "serein/interface/app_icon.h"
#include "serein/settings/gen/interface_rows.h"
#include "serein/settings/home.h"
#include "serein/settings/restart.h"
#include "serein/settings/lock.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class InterfaceSection final : public Section<InterfaceSection> {
public:
	InterfaceSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		Serein::GuardSettings(content, [=] { build(content, kBuild); });
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_interface();
	}

	static const SectionBuildMethod kBuild;
};

QString RoundnessLabel(int value) {
	return (value
		? QString::number(value) + u"%"_q
		: tr::lng_serein_preview_follow(tr::now))
		+ u" · "_q + tr::lng_serein_restart_required(tr::now);
}

void RoundnessBox(
		not_null<Ui::GenericBox*> box,
		const Option<int> &option,
		QString title,
		not_null<Window::SessionController*> controller) {
	box->setTitle(std::move(title));
	const auto current = ForDevice().Get(option);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		tr::lng_serein_roundness_hint(),
		current ? QString::number(current) : QString()));
	field->setInputMethodHints(Qt::ImhDigitsOnly);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto submit = [=] {
		const auto text = field->getLastText().trimmed();
		auto valid = false;
		const auto value = text.isEmpty() ? 0 : text.toInt(&valid);
		if ((!text.isEmpty() && !valid)
			|| (value != 0 && (value < 10 || value > 100))) {
			field->showError();
			return;
		}
		if (value != current) {
			Expects(ForDevice().Set(option, value));
			ShowRestartPrompt(controller);
		}
		box->closeBox();
	};
	field->submits(
	) | rpl::on_next([=](auto) { submit(); }, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void AddRoundness(
		SectionBuilder &builder,
		const Option<int> &option,
		rpl::producer<QString> title,
		QString id,
		QStringList keywords) {
	const auto controller = builder.controller();
	builder.addButton({
		.id = std::move(id),
		.title = std::move(title),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(option) | rpl::map(RoundnessLabel),
		.onClick = [=] {
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				RoundnessBox(box, option, (option.key == Interface::kBubbleRoundness.key)
					? tr::lng_serein_bubble_roundness(tr::now)
					: tr::lng_serein_avatar_roundness(tr::now), controller);
			}));
		},
		.keywords = std::move(keywords),
	});
}

QString DelayLabel(int milliseconds) {
	return milliseconds
		? QString::number(milliseconds / 1000.0)
			+ tr::lng_serein_seconds_suffix(tr::now)
		: tr::lng_serein_preview_follow(tr::now);
}

void DelayBox(
		not_null<Ui::GenericBox*> box,
		const Option<int> &option,
		QString title) {
	box->setTitle(std::move(title));
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		ForDevice().Get(option));
	for (const auto value : { 0, 500, 1000, 2000, 5000, 10000, 30000, 60000 }) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value, DelayLabel(value), st::settingsSendType),
			st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(ForDevice().Set(option, value));
		box->closeBox();
	});
}

void AddDelay(
		SectionBuilder &builder,
		const Option<int> &option,
		rpl::producer<QString> title,
		QString id) {
	const auto controller = builder.controller();
	builder.addButton({
		.id = std::move(id),
		.title = std::move(title),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(option) | rpl::map(DelayLabel),
		.onClick = [=] {
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				DelayBox(box, option, option.key == Interface::kNotificationDelay.key
					? tr::lng_serein_notification_delay(tr::now)
					: tr::lng_serein_other_device_notification_delay(tr::now));
			}));
		},
		.keywords = { u"notification"_q, u"delay"_q },
	});
}

const auto kMeta = BuildHelper({
	.id = InterfaceSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_interface,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	Interface::AddLayout(builder, {
		.bubbleRoundness = [&] {
			AddRoundness(builder, Interface::kBubbleRoundness,
				tr::lng_serein_bubble_roundness(),
				u"serein/interface/bubble-roundness"_q,
				{ u"bubble"_q, u"roundness"_q });
		},
		.avatarRoundness = [&] {
			AddRoundness(builder, Interface::kAvatarRoundness,
				tr::lng_serein_avatar_roundness(),
				u"serein/interface/avatar-roundness"_q,
				{ u"avatar"_q, u"roundness"_q });
		},
		.uniformAvatarShapes = [&] {
			const auto button = builder.addButton({
				.id = u"serein/interface/uniform-avatars"_q,
				.title = tr::lng_serein_uniform_avatar_shapes(),
				.st = &st::settingsButtonNoIcon,
				.label = rpl::single(tr::lng_serein_restart_required(tr::now)),
				.toggled = ForDevice().Value(Interface::kUniformAvatarShapes),
				.keywords = { u"forum"_q, u"channel"_q, u"avatar"_q },
				.shown = ForDevice().Value(Interface::kAvatarRoundness)
					| rpl::map([](int value) { return value != 0; }),
			});
			if (button) {
				button->toggledChanges(
				) | rpl::on_next([=](bool value) {
					Expects(ForDevice().Set(Interface::kUniformAvatarShapes, value));
					ShowRestartPrompt(controller);
				}, button->lifetime());
			}
		},
		.mainMenu = [&] {
			builder.addButton({
				.id = u"serein/interface/main-menu"_q,
				.title = tr::lng_serein_main_menu(),
				.st = &st::settingsButtonNoIcon,
				.onClick = [=] { controller->show(Box(Interface::MainMenuBox)); },
				.keywords = { u"menu"_q, u"order"_q, u"visibility"_q },
			});
			builder.addButton({
				.id = u"serein/interface/app-icon"_q,
				.title = tr::lng_serein_app_icon(),
				.st = &st::settingsButtonNoIcon,
				.label = Interface::CustomAppIconValue(
				) | rpl::map([](bool custom) {
					return custom
						? tr::lng_serein_app_icon_custom(tr::now)
						: tr::lng_serein_app_icon_default(tr::now);
				}),
				.onClick = [=] { controller->show(Box(Interface::AppIconBox)); },
				.keywords = { u"icon"_q, u"dock"_q, u"taskbar"_q },
			});
		},
		.notificationDelay = [&] {
			AddDelay(builder, Interface::kNotificationDelay,
				tr::lng_serein_notification_delay(),
				u"serein/interface/notification-delay"_q);
		},
		.otherDeviceNotificationDelay = [&] {
			AddDelay(builder, Interface::kOtherDeviceNotificationDelay,
				tr::lng_serein_other_device_notification_delay(),
				u"serein/interface/other-device-notification-delay"_q);
		},
	});
});

const SectionBuildMethod InterfaceSection::kBuild = kMeta.build;

} // namespace

Settings::Type InterfaceId() {
	return InterfaceSection::Id();
}

} // namespace Serein
