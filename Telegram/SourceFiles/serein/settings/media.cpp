#include "serein/settings/media.h"

#include "serein/media/options.h"
#include "serein/media/sticker_catalog.h"
#include "serein/settings/gen/media_rows.h"
#include "serein/settings/home.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class MediaSection final : public Section<MediaSection> {
public:
	MediaSection(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_media();
	}

	static const SectionBuildMethod kBuild;
};

void StickerScaleBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_sticker_scale());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		ForDevice().Get(Media::kStickerScale));
	for (auto value = 50; value <= 200; value += 25) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value, QString::number(value) + '%',
			st::settingsSendType), st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(ForDevice().Set(Media::kStickerScale, value));
		box->closeBox();
	});
}

void RecentLimitBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_recent_sticker_limit());
	const auto current = ForDevice().Get(Media::kRecentStickerLimit);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		tr::lng_serein_recent_sticker_hint(),
		current ? QString::number(current) : QString()));
	field->setInputMethodHints(Qt::ImhDigitsOnly);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto submit = [=] {
		const auto text = field->getLastText().trimmed();
		auto valid = false;
		const auto value = text.isEmpty() ? 0 : text.toInt(&valid);
		if (!text.isEmpty() && (!valid || value < 1 || value > 200)) {
			field->showError();
			return;
		}
		Expects(ForDevice().Set(Media::kRecentStickerLimit, value));
		box->closeBox();
	};
	field->submits(
	) | rpl::on_next([=](auto) { submit(); }, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

const auto kMeta = BuildHelper({
	.id = MediaSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_media,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/media/sticker-scale"_q,
		.title = tr::lng_serein_sticker_scale(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Media::kStickerScale)
			| rpl::map([](int value) {
				return QString::number(value) + '%';
			}),
		.onClick = [=] { controller->show(Box(StickerScaleBox)); },
		.keywords = { u"sticker"_q, u"size"_q },
	});
	builder.addButton({
		.id = u"serein/media/recent-sticker-limit"_q,
		.title = tr::lng_serein_recent_sticker_limit(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Media::kRecentStickerLimit)
			| rpl::map([](int value) {
				return value ? QString::number(value)
					: tr::lng_serein_preview_follow(tr::now);
			}),
		.onClick = [=] { controller->show(Box(RecentLimitBox)); },
		.keywords = { u"recent"_q, u"sticker"_q, u"limit"_q },
	});
	AddToggles(builder, Media::kToggleRows);
	builder.addButton({
		.id = u"serein/media/sticker-catalog"_q,
		.title = tr::lng_serein_catalog_title(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { ShowStickerCatalog(controller); },
		.keywords = { u"sticker"_q, u"catalog"_q },
	});
});

const SectionBuildMethod MediaSection::kBuild = kMeta.build;

} // namespace

Settings::Type MediaId() {
	return MediaSection::Id();
}

} // namespace Serein
