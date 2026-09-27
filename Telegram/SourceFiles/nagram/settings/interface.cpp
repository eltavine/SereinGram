#include "nagram/settings/interface.h"

#include "nagram/interface/options.h"
#include "nagram/interface/main_menu.h"
#include "nagram/settings/home.h"
#include "nagram/settings/restart.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Nagram {
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
		return tr::lng_nagram_interface();
	}

	static const SectionBuildMethod kBuild;
};

QString RoundnessLabel(int value) {
	return (value
		? QString::number(value) + u"%"_q
		: tr::lng_nagram_preview_follow(tr::now))
		+ u" · "_q + tr::lng_nagram_restart_required(tr::now);
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
		tr::lng_nagram_roundness_hint(),
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
					? tr::lng_nagram_bubble_roundness(tr::now)
					: tr::lng_nagram_avatar_roundness(tr::now), controller);
			}));
		},
		.keywords = std::move(keywords),
	});
}

void TextWidthBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_nagram_text_message_width());
	const auto current = ForDevice().Get(Interface::kTextMessageWidth);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		tr::lng_nagram_text_width_hint(),
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

const auto kMeta = BuildHelper({
	.id = InterfaceSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_nagram_interface,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	builder.addSubsectionTitle({
		.id = u"nagram/interface/roundness"_q,
		.title = tr::lng_nagram_roundness_and_shapes(),
		.keywords = { u"corners"_q, u"shapes"_q },
	});
	AddRoundness(builder, Interface::kBubbleRoundness,
		tr::lng_nagram_bubble_roundness(),
		u"nagram/interface/bubble-roundness"_q,
		{ u"bubble"_q, u"roundness"_q });
	AddRoundness(builder, Interface::kAvatarRoundness,
		tr::lng_nagram_avatar_roundness(),
		u"nagram/interface/avatar-roundness"_q,
		{ u"avatar"_q, u"roundness"_q });
	const auto controller = builder.controller();
	const auto button = builder.addButton({
		.id = u"nagram/interface/uniform-avatars"_q,
		.title = tr::lng_nagram_uniform_avatar_shapes(),
		.st = &st::settingsButtonNoIcon,
		.label = rpl::single(tr::lng_nagram_restart_required(tr::now)),
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
		.id = u"nagram/interface/message-style"_q,
		.title = tr::lng_nagram_message_style(),
		.keywords = { u"messages"_q, u"style"_q },
	});
	builder.addButton({
		.id = u"nagram/interface/text-width"_q,
		.title = tr::lng_nagram_text_message_width(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Interface::kTextMessageWidth)
			| rpl::map([](int value) {
				return value ? QString::number(value) + u"%"_q
					: tr::lng_nagram_preview_follow(tr::now);
			}),
		.onClick = [=] { controller->show(Box(TextWidthBox)); },
		.keywords = { u"text"_q, u"width"_q },
	});
	AddToggle(builder, Interface::kWideChannelPosts,
		tr::lng_nagram_wide_channel_posts(),
		u"nagram/interface/wide-channel-posts"_q,
		{ u"channel"_q, u"width"_q });
	AddToggle(builder, Interface::kHideBubbleTail,
		tr::lng_nagram_hide_bubble_tail(),
		u"nagram/interface/hide-bubble-tail"_q,
		{ u"bubble"_q, u"tail"_q });
	AddToggle(builder, Interface::kThemeReplyColors,
		tr::lng_nagram_theme_reply_colors(),
		u"nagram/interface/theme-reply-colors"_q,
		{ u"reply"_q, u"quote"_q, u"color"_q });
	AddToggle(builder, Interface::kHideReplyThumbnail,
		tr::lng_nagram_hide_reply_thumbnail(),
		u"nagram/interface/hide-reply-thumbnail"_q,
		{ u"reply"_q, u"thumbnail"_q });
	AddToggle(builder, Interface::kIgnoreChatTheme,
		tr::lng_nagram_ignore_chat_theme(),
		u"nagram/interface/ignore-chat-theme"_q,
		{ u"chat"_q, u"theme"_q, u"wallpaper"_q });
	builder.addSubsectionTitle({
		.id = u"nagram/interface/main-menu-heading"_q,
		.title = tr::lng_nagram_main_menu(),
		.keywords = { u"menu"_q, u"title"_q },
	});
	builder.addButton({
		.id = u"nagram/interface/main-menu"_q,
		.title = tr::lng_nagram_main_menu(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { controller->show(Box(Interface::MainMenuBox)); },
		.keywords = { u"menu"_q, u"order"_q, u"visibility"_q },
	});
});

const SectionBuildMethod InterfaceSection::kBuild = kMeta.build;

} // namespace

Settings::Type InterfaceId() {
	return InterfaceSection::Id();
}

} // namespace Nagram
