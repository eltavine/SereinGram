#include "serein/settings/interface.h"

#include "serein/interface/options.h"
#include "serein/interface/main_menu.h"
#include "serein/settings/home.h"
#include "serein/settings/restart.h"
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
		build(content, kBuild);
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

void TextWidthBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_text_message_width());
	const auto current = ForDevice().Get(Interface::kTextMessageWidth);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		tr::lng_serein_text_width_hint(),
		current ? QString::number(current) : QString()));
	field->setInputMethodHints(Qt::ImhDigitsOnly);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto submit = [=] {
		const auto text = field->getLastText().trimmed();
		auto valid = false;
		const auto value = text.isEmpty() ? 0 : text.toInt(&valid);
		if ((!text.isEmpty() && !valid)
			|| (value != 0 && (value < 50 || value > 400))) {
			field->showError();
			return;
		}
		if (value != current) {
			Expects(ForDevice().Set(Interface::kTextMessageWidth, value));
		}
		box->closeBox();
	};
	field->submits(
	) | rpl::on_next([=](auto) { submit(); }, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
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

void AddToggle(
		SectionBuilder &builder,
		const Option<bool> &option,
		rpl::producer<QString> title,
		QString id,
		QStringList keywords) {
	const auto controller = builder.controller();
	const auto button = builder.addButton({
		.id = std::move(id),
		.title = std::move(title),
		.st = &st::settingsButtonNoIcon,
		.toggled = ForDevice().Value(option),
		.keywords = std::move(keywords),
	});
	if (button) {
		button->toggledChanges(
		) | rpl::on_next([option, controller](bool value) {
			Expects(ForDevice().Set(option, value));
			if (option.flags & Interface::kRestart) {
				ShowRestartPrompt(controller);
			}
		}, button->lifetime());
	}
}

const auto kMeta = BuildHelper({
	.id = InterfaceSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_interface,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	builder.addSubsectionTitle({
		.id = u"serein/interface/roundness"_q,
		.title = tr::lng_serein_roundness_and_shapes(),
		.keywords = { u"corners"_q, u"shapes"_q },
	});
	AddRoundness(builder, Interface::kBubbleRoundness,
		tr::lng_serein_bubble_roundness(),
		u"serein/interface/bubble-roundness"_q,
		{ u"bubble"_q, u"roundness"_q });
	AddRoundness(builder, Interface::kAvatarRoundness,
		tr::lng_serein_avatar_roundness(),
		u"serein/interface/avatar-roundness"_q,
		{ u"avatar"_q, u"roundness"_q });
	const auto controller = builder.controller();
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
	builder.addSubsectionTitle({
		.id = u"serein/interface/message-style"_q,
		.title = tr::lng_serein_message_style(),
		.keywords = { u"messages"_q, u"style"_q },
	});
	builder.addButton({
		.id = u"serein/interface/text-width"_q,
		.title = tr::lng_serein_text_message_width(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Interface::kTextMessageWidth)
			| rpl::map([](int value) {
				return value ? QString::number(value) + u"%"_q
					: tr::lng_serein_preview_follow(tr::now);
			}),
		.onClick = [=] { controller->show(Box(TextWidthBox)); },
		.keywords = { u"text"_q, u"width"_q },
	});
	AddToggle(builder, Interface::kWideChannelPosts,
		tr::lng_serein_wide_channel_posts(),
		u"serein/interface/wide-channel-posts"_q,
		{ u"channel"_q, u"width"_q });
	AddToggle(builder, Interface::kHideBubbleTail,
		tr::lng_serein_hide_bubble_tail(),
		u"serein/interface/hide-bubble-tail"_q,
		{ u"bubble"_q, u"tail"_q });
	AddToggle(builder, Interface::kThemeReplyColors,
		tr::lng_serein_theme_reply_colors(),
		u"serein/interface/theme-reply-colors"_q,
		{ u"reply"_q, u"quote"_q, u"color"_q });
	AddToggle(builder, Interface::kHideReplyThumbnail,
		tr::lng_serein_hide_reply_thumbnail(),
		u"serein/interface/hide-reply-thumbnail"_q,
		{ u"reply"_q, u"thumbnail"_q });
	AddToggle(builder, Interface::kIgnoreChatTheme,
		tr::lng_serein_ignore_chat_theme(),
		u"serein/interface/ignore-chat-theme"_q,
		{ u"chat"_q, u"theme"_q, u"wallpaper"_q });
	builder.addSubsectionTitle({
		.id = u"serein/interface/main-menu-heading"_q,
		.title = tr::lng_serein_main_menu(),
		.keywords = { u"menu"_q, u"title"_q },
	});
	builder.addButton({
		.id = u"serein/interface/main-menu"_q,
		.title = tr::lng_serein_main_menu(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { controller->show(Box(Interface::MainMenuBox)); },
		.keywords = { u"menu"_q, u"order"_q, u"visibility"_q },
	});
	builder.addSubsectionTitle({
		.id = u"serein/interface/window-notification"_q,
		.title = tr::lng_serein_window_notification(),
		.keywords = { u"window"_q, u"notification"_q },
	});
	AddToggle(builder, Interface::kHideAppIconBadge,
		tr::lng_serein_hide_app_icon_badge(),
		u"serein/interface/hide-app-icon-badge"_q,
		{ u"dock"_q, u"icon"_q, u"badge"_q });
	AddDelay(builder, Interface::kNotificationDelay,
		tr::lng_serein_notification_delay(),
		u"serein/interface/notification-delay"_q);
	AddDelay(builder, Interface::kOtherDeviceNotificationDelay,
		tr::lng_serein_other_device_notification_delay(),
		u"serein/interface/other-device-notification-delay"_q);
	builder.addSubsectionTitle({
		.id = u"serein/interface/text"_q,
		.title = tr::lng_serein_ui_text(),
		.keywords = { u"text"_q, u"punctuation"_q },
	});
	AddToggle(builder, Interface::kHalfwidthUiPunctuation,
		tr::lng_serein_halfwidth_ui_punctuation(),
		u"serein/interface/halfwidth-ui-punctuation"_q,
		{ u"text"_q, u"punctuation"_q });
});

const SectionBuildMethod InterfaceSection::kBuild = kMeta.build;

} // namespace

Settings::Type InterfaceId() {
	return InterfaceSection::Id();
}

} // namespace Serein
