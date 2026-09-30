#include "serein/settings/messages.h"

#include "serein/core/options.h"
#include "serein/messages/options.h"
#include "serein/messages/reading.h"
#include "serein/settings/gen/messages_rows.h"
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
#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class MessagesSection final : public Section<MessagesSection> {
public:
	MessagesSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_messages();
	}

	static const SectionBuildMethod kBuild;
};

void EditMarkBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_edited_mark());
	const auto field = box->addRow(
		object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			tr::lng_edited(),
			ForDevice().Get(Messages::kEditedMark)),
		st::boxRowPadding);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto submit = [=] {
		if (ForDevice().Set(Messages::kEditedMark, field->getLastText())) {
			box->closeBox();
		}
	};
	field->submits() | rpl::on_next(submit, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

QString ReadingChineseLabel(int value) {
	switch (value) {
	case 1: return tr::lng_serein_reading_simplified(tr::now);
	case 2: return tr::lng_serein_reading_traditional(tr::now);
	default: return tr::lng_serein_reading_off(tr::now);
	}
}

void ReadingChineseBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_reading_chinese());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		ForDevice().Get(Messages::kReadingChinese));
	for (auto value = 0; value != 3; ++value) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value, ReadingChineseLabel(value),
			st::settingsSendType), st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(ForDevice().Set(Messages::kReadingChinese, value));
		box->closeBox();
	});
}

const auto kMeta = BuildHelper({
	.id = MessagesSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_messages,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	Messages::AddLayout(builder, {
		.editedMark = [&] {
			builder.addButton({
				.id = u"serein/messages/edited-mark"_q,
				.title = tr::lng_serein_edited_mark(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Messages::kEditedMark)
					| rpl::map([](const QString &text) {
						return text.isEmpty()
							? tr::lng_settings_notifications_display_default(tr::now)
							: text;
					}),
				.onClick = [=] { controller->show(Box(EditMarkBox)); },
				.keywords = { u"edited"_q, u"label"_q, u"text"_q },
				.shown = ForDevice().Value(Messages::kHideEditedBadge)
					| rpl::map([](bool hidden) { return !hidden; }),
			});
		},
		.readingChinese = [&] {
			const auto available = Messages::ChineseConversionAvailable();
			const auto chinese = builder.addButton({
				.id = u"serein/messages/reading-chinese"_q,
				.title = tr::lng_serein_reading_chinese(),
				.st = &st::settingsButtonNoIcon,
				.label = available
					? rpl::producer<QString>(
						ForDevice().Value(Messages::kReadingChinese)
						| rpl::map(ReadingChineseLabel))
					: rpl::producer<QString>(rpl::single(
						tr::lng_serein_reading_unavailable(tr::now))),
				.onClick = [=] { controller->show(Box(ReadingChineseBox)); },
				.keywords = { u"Chinese"_q, u"simplified"_q, u"traditional"_q },
			});
			if (chinese && !available) {
				chinese->setDisabled(true);
			}
		},
	});
});

const SectionBuildMethod MessagesSection::kBuild = kMeta.build;

} // namespace

Settings::Type MessagesId() {
	return MessagesSection::Id();
}

} // namespace Serein
