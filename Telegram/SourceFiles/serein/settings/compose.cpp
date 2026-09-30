#include "serein/settings/compose.h"

#include "serein/compose/options.h"
#include "serein/compose/text.h"
#include "serein/settings/home.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_layers.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class ComposeSection final : public Section<ComposeSection> {
public:
	ComposeSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_compose();
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

QString PlaceholderLabel(int value) {
	switch (value) {
	case 1: return tr::lng_serein_placeholder_chat(tr::now);
	case 2: return tr::lng_serein_placeholder_sender(tr::now);
	default: return tr::lng_serein_preview_follow(tr::now);
	}
}

void PlaceholderBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_input_placeholder());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		ForDevice().Get(Compose::kInputPlaceholderMode));
	for (auto value = 0; value != 3; ++value) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value, PlaceholderLabel(value), st::settingsSendType),
			st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(ForDevice().Set(Compose::kInputPlaceholderMode, value));
		box->closeBox();
	});
}

void CodeLanguageBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_default_code_language());
	const auto current = ForDevice().Get(Compose::kDefaultCodeLanguage);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField,
		tr::lng_serein_code_language_hint(), current));
	field->setMaxLength(32);
	field->setInputMethodHints(Qt::ImhLatinOnly
		| Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto save = [=] {
		const auto value = field->getLastText().trimmed();
		if (!Compose::kDefaultCodeLanguage.validate(value)) {
			field->showError();
			return;
		}
		Expects(ForDevice().Set(Compose::kDefaultCodeLanguage, value));
		box->closeBox();
	};
	field->submits(
	) | rpl::on_next([=](auto) { save(); }, field->lifetime());
	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void QuickRepliesBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_quick_replies());
	auto current = Compose::QuickReplies();
	if (current.size() != 2) {
		current = { QString(), QString() };
	}
	const auto first = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField,
		tr::lng_serein_quick_reply_one(), current[0]));
	const auto second = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField,
		tr::lng_serein_quick_reply_two(), current[1]));
	box->setFocusCallback([=] { first->setFocusFast(); });
	box->addButton(tr::lng_settings_save(), [=] {
		Expects(Compose::SetQuickReplies({
			first->getLastText(), second->getLastText() }));
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

const auto kMeta = BuildHelper({
	.id = ComposeSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_compose,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	builder.addSubsectionTitle({
		.id = u"serein/compose/buttons"_q,
		.title = tr::lng_serein_compose_buttons(),
		.keywords = { u"buttons"_q, u"compose"_q },
	});
	AddToggle(builder, Compose::kHideAttachButton,
		tr::lng_serein_hide_attach_button(),
		u"serein/compose/hide-attach"_q, { u"attach"_q });
	AddToggle(builder, Compose::kHideEmojiButton,
		tr::lng_serein_hide_emoji_button(),
		u"serein/compose/hide-emoji"_q, { u"emoji"_q });
	AddToggle(builder, Compose::kHideRecordingButton,
		tr::lng_serein_hide_recording_button(),
		u"serein/compose/hide-recording"_q, { u"recording"_q });
	AddToggle(builder, Compose::kHideBotCommandButton,
		tr::lng_serein_hide_bot_command_button(),
		u"serein/compose/hide-bot-command"_q, { u"bot"_q, u"command"_q });
	AddToggle(builder, Compose::kHideBotMenu,
		tr::lng_serein_hide_bot_menu(),
		u"serein/compose/hide-bot-menu"_q, { u"bot"_q, u"menu"_q });
	AddToggle(builder, Compose::kHideAutoDeleteButton,
		tr::lng_serein_hide_auto_delete_button(),
		u"serein/compose/hide-auto-delete"_q, { u"delete"_q, u"timer"_q });
	AddToggle(builder, Compose::kHideGiftButton,
		tr::lng_serein_hide_gift_button(),
		u"serein/compose/hide-gift"_q, { u"gift"_q });
	AddToggle(builder, Compose::kHideAiButton,
		tr::lng_serein_hide_ai_button(),
		u"serein/compose/hide-ai"_q, { u"AI"_q });
	AddToggle(builder, Compose::kHideSendAsButton,
		tr::lng_serein_hide_send_as_button(),
		u"serein/compose/hide-send-as"_q, { u"send as"_q });
	AddToggle(builder, Compose::kHideStarsReactionButton,
		tr::lng_serein_hide_stars_reaction_button(),
		u"serein/compose/hide-stars-reaction"_q, { u"Stars"_q });
	AddToggle(builder, Compose::kHideChannelMuteButton,
		tr::lng_serein_hide_channel_mute_button(),
		u"serein/compose/hide-channel-mute"_q, { u"channel"_q, u"mute"_q });
	builder.addSubsectionTitle({
		.id = u"serein/compose/input-behavior"_q,
		.title = tr::lng_serein_input_behavior(),
		.keywords = { u"input"_q, u"behavior"_q },
	});
	AddToggle(builder, Compose::kDisableEmojiHover,
		tr::lng_serein_disable_emoji_hover(),
		u"serein/compose/disable-emoji-hover"_q,
		{ u"emoji"_q, u"hover"_q });
	AddToggle(builder, Compose::kDisableAttachHover,
		tr::lng_serein_disable_attach_hover(),
		u"serein/compose/disable-attach-hover"_q,
		{ u"attachment"_q, u"hover"_q });
	AddToggle(builder, Compose::kBotCommandsToDraft,
		tr::lng_serein_bot_commands_to_draft(),
		u"serein/compose/bot-commands-to-draft"_q,
		{ u"bot"_q, u"command"_q, u"draft"_q });
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/compose/input-placeholder"_q,
		.title = tr::lng_serein_input_placeholder(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Compose::kInputPlaceholderMode)
			| rpl::map(PlaceholderLabel),
		.onClick = [=] { controller->show(Box(PlaceholderBox)); },
		.keywords = { u"placeholder"_q, u"hint"_q },
	});
	builder.addSubsectionTitle({
		.id = u"serein/compose/text-format"_q,
		.title = tr::lng_serein_text_format(),
		.keywords = { u"text"_q, u"format"_q },
	});
	AddToggle(builder, Compose::kDisableAutoMarkdown,
		tr::lng_serein_disable_auto_markdown(),
		u"serein/compose/disable-auto-markdown"_q,
		{ u"Markdown"_q });
	AddToggle(builder, Compose::kDisableLinkPreview,
		tr::lng_serein_disable_link_preview(),
		u"serein/compose/disable-link-preview"_q,
		{ u"link"_q, u"preview"_q });
	AddToggle(builder, Compose::kSpaceOnSend,
		tr::lng_serein_space_on_send(),
		u"serein/compose/space-on-send"_q,
		{ u"spacing"_q, u"send"_q });
	AddToggle(builder, Compose::kSpaceOnEdit,
		tr::lng_serein_space_on_edit(),
		u"serein/compose/space-on-edit"_q,
		{ u"spacing"_q, u"edit"_q });
	builder.addButton({
		.id = u"serein/compose/default-code-language"_q,
		.title = tr::lng_serein_default_code_language(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Compose::kDefaultCodeLanguage)
			| rpl::map([](const QString &value) {
				return value.isEmpty()
					? tr::lng_serein_preview_follow(tr::now)
					: value;
			}),
		.onClick = [=] { controller->show(Box(CodeLanguageBox)); },
		.keywords = { u"code"_q, u"language"_q },
	});
	builder.addButton({
		.id = u"serein/compose/quick-replies"_q,
		.title = tr::lng_serein_quick_replies(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { controller->show(Box(QuickRepliesBox)); },
		.keywords = { u"quick"_q, u"reply"_q },
	});
	builder.addSubsectionTitle({
		.id = u"serein/compose/send-confirmation"_q,
		.title = tr::lng_serein_send_confirmation(),
		.keywords = { u"send"_q, u"confirm"_q },
	});
	AddToggle(builder, Compose::kConfirmSticker,
		tr::lng_serein_confirm_sticker(),
		u"serein/compose/confirm-sticker"_q,
		{ u"sticker"_q, u"confirm"_q });
	AddToggle(builder, Compose::kConfirmGif,
		tr::lng_serein_confirm_gif(),
		u"serein/compose/confirm-gif"_q,
		{ u"GIF"_q, u"confirm"_q });
	AddToggle(builder, Compose::kPreviewVoice,
		tr::lng_serein_preview_voice(),
		u"serein/compose/preview-voice"_q,
		{ u"voice"_q, u"listen"_q });
	AddToggle(builder, Compose::kPreviewRoundVideo,
		tr::lng_serein_preview_round_video(),
		u"serein/compose/preview-round-video"_q,
		{ u"video"_q, u"preview"_q });
	AddToggle(builder, Compose::kConfirmPrivateCall,
		tr::lng_serein_confirm_private_call(),
		u"serein/compose/confirm-private-call"_q,
		{ u"call"_q, u"confirm"_q });
	builder.addSubsectionTitle({
		.id = u"serein/compose/forwarding"_q,
		.title = tr::lng_serein_forwarding(),
		.keywords = { u"forward"_q },
	});
	AddToggle(builder, Compose::kForwardBeforeComment,
		tr::lng_serein_forward_before_comment(),
		u"serein/compose/forward-before-comment"_q,
		{ u"forward"_q, u"comment"_q, u"order"_q });
});

const SectionBuildMethod ComposeSection::kBuild = kMeta.build;

} // namespace

Settings::Type ComposeId() {
	return ComposeSection::Id();
}

} // namespace Serein
