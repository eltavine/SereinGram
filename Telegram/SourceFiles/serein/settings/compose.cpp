#include "serein/settings/compose.h"

#include "serein/compose/link_inline_bots.h"
#include "serein/compose/options.h"
#include "serein/compose/text_replacements.h"
#include "serein/hooks/compose/text.h"
#include "serein/settings/gen/compose_rows.h"
#include "serein/settings/home.h"
#include "serein/settings/page.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_layers.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class ComposeSection final : public Page<ComposeSection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_compose();
	}

	static const SectionBuildMethod kBuild;

};

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

void TextReplacementsBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_text_replacements());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_text_replacements_about(),
		st::boxLabel));
	const auto current = Compose::ReadTextReplacements(
		ForDevice().Get(Compose::kTextReplacements)
	).value_or(Compose::TextReplacements());
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::MultiLine,
		rpl::single(u"brb => be right back"_q),
		Compose::FormatReplacementLines(current)));
	field->setMaxLength(40000);
	box->setFocusCallback([=] { field->setFocusFast(); });
	box->addButton(tr::lng_settings_save(), [=] {
		const auto parsed = Compose::ParseReplacementLines(
			field->getLastText());
		if (!parsed) {
			field->showError();
			box->showToast(tr::lng_serein_text_replacements_invalid(tr::now));
			return;
		}
		Expects(ForDevice().Set(
			Compose::kTextReplacements,
			Compose::WriteTextReplacements(*parsed)));
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void LinkInlineBotsBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_link_inline_bots());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_link_inline_bots_about(),
		st::boxLabel));
	const auto current = Compose::ReadLinkInlineBots(
		ForDevice().Get(Compose::kLinkInlineBots)
	).value_or(Compose::LinkInlineBots());
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::MultiLine,
		rpl::single(u"@vid => youtube\\.com/|youtu\\.be/"_q),
		Compose::FormatLinkInlineBotLines(current)));
	field->setMaxLength(20000);
	box->setFocusCallback([=] { field->setFocusFast(); });
	box->addButton(tr::lng_settings_save(), [=] {
		const auto parsed = Compose::ParseLinkInlineBotLines(
			field->getLastText());
		if (!parsed) {
			field->showError();
			box->showToast(tr::lng_serein_link_inline_bots_invalid(tr::now));
			return;
		}
		Expects(ForDevice().Set(
			Compose::kLinkInlineBots,
			Compose::WriteLinkInlineBots(*parsed)));
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
	const auto controller = builder.controller();
	Compose::AddLayout(builder, {
		.defaultCodeLanguage = [&] {
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
		},
		.quickReplies = [&] {
			builder.addButton({
				.id = u"serein/compose/quick-replies"_q,
				.title = tr::lng_serein_quick_replies(),
				.st = &st::settingsButtonNoIcon,
				.onClick = [=] { controller->show(Box(QuickRepliesBox)); },
				.keywords = { u"quick"_q, u"reply"_q },
			});
		},
		.textReplacements = [&] {
			builder.addButton({
				.id = u"serein/compose/text-replacements"_q,
				.title = tr::lng_serein_text_replacements(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Compose::kTextReplacements)
					| rpl::map([](const QByteArray &raw) {
						const auto rules = Compose::ReadTextReplacements(raw);
						const auto count = rules ? int(rules->rules.size()) : 0;
						return count
							? QString::number(count)
							: tr::lng_serein_config_off(tr::now);
					}),
				.onClick = [=] {
					controller->show(Box(TextReplacementsBox));
				},
				.keywords = { u"replace"_q, u"shortcut"_q, u"text"_q },
			});
		},
		.linkInlineBots = [&] {
			builder.addButton({
				.id = u"serein/compose/link-inline-bots"_q,
				.title = tr::lng_serein_link_inline_bots(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Compose::kLinkInlineBots)
					| rpl::map([](const QByteArray &raw) {
						const auto rules = Compose::ReadLinkInlineBots(raw);
						const auto count = rules ? int(rules->rules.size()) : 0;
						return count
							? QString::number(count)
							: tr::lng_serein_config_off(tr::now);
					}),
				.onClick = [=] {
					controller->show(Box(LinkInlineBotsBox));
				},
				.keywords = { u"inline"_q, u"bot"_q, u"link"_q },
			});
		},
	});
});

const SectionBuildMethod ComposeSection::kBuild = kMeta.build;

} // namespace

Settings::Type ComposeId() {
	return ComposeSection::Id();
}

} // namespace Serein
