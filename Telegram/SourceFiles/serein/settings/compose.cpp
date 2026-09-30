#include "serein/settings/compose.h"

#include "serein/compose/options.h"
#include "serein/hooks/compose/text.h"
#include "serein/settings/gen/compose_rows.h"
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
	const auto controller = builder.controller();
	Compose::AddLayout(builder, {
		.inputPlaceholderMode = [&] {
			builder.addButton({
				.id = u"serein/compose/input-placeholder"_q,
				.title = tr::lng_serein_input_placeholder(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Compose::kInputPlaceholderMode)
					| rpl::map(PlaceholderLabel),
				.onClick = [=] { controller->show(Box(PlaceholderBox)); },
				.keywords = { u"placeholder"_q, u"hint"_q },
			});
		},
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
	});
});

const SectionBuildMethod ComposeSection::kBuild = kMeta.build;

} // namespace

Settings::Type ComposeId() {
	return ComposeSection::Id();
}

} // namespace Serein
