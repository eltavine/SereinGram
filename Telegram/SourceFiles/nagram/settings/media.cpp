#include "nagram/settings/media.h"

#include "nagram/media/options.h"
#include "nagram/media/sticker_catalog.h"
#include "nagram/settings/home.h"
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

namespace Nagram {
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
		return tr::lng_nagram_media();
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

void StickerScaleBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_nagram_sticker_scale());
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
	box->setTitle(tr::lng_nagram_recent_sticker_limit());
	const auto current = ForDevice().Get(Media::kRecentStickerLimit);
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		tr::lng_nagram_recent_sticker_hint(),
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
	.title = &tr::lng_nagram_media,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"nagram/media/sticker-scale"_q,
		.title = tr::lng_nagram_sticker_scale(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Media::kStickerScale)
			| rpl::map([](int value) {
				return QString::number(value) + '%';
			}),
		.onClick = [=] { controller->show(Box(StickerScaleBox)); },
		.keywords = { u"sticker"_q, u"size"_q },
	});
	AddToggle(builder, Media::kHideStickerTime,
		tr::lng_nagram_hide_sticker_time(),
		u"nagram/media/hide-sticker-time"_q,
		{ u"sticker"_q, u"time"_q });
	builder.addButton({
		.id = u"nagram/media/recent-sticker-limit"_q,
		.title = tr::lng_nagram_recent_sticker_limit(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Media::kRecentStickerLimit)
			| rpl::map([](int value) {
				return value ? QString::number(value)
					: tr::lng_nagram_preview_follow(tr::now);
			}),
		.onClick = [=] { controller->show(Box(RecentLimitBox)); },
		.keywords = { u"recent"_q, u"sticker"_q, u"limit"_q },
	});
	AddToggle(builder, Media::kHideGroupStickers,
		tr::lng_nagram_hide_group_stickers(),
		u"nagram/media/hide-group-stickers"_q,
		{ u"group"_q, u"sticker"_q });
	AddToggle(builder, Media::kHideRecommendedStickers,
		tr::lng_nagram_hide_recommended_stickers(),
		u"nagram/media/hide-recommended-stickers"_q,
		{ u"recommended"_q, u"sticker"_q });
	AddToggle(builder, Media::kHideRecommendedEmoji,
		tr::lng_nagram_hide_recommended_emoji(),
		u"nagram/media/hide-recommended-emoji"_q,
		{ u"recommended"_q, u"emoji"_q });
	AddToggle(builder, Media::kHideGifCategories,
		tr::lng_nagram_hide_gif_categories(),
		u"nagram/media/hide-gif-categories"_q,
		{ u"GIF"_q, u"categories"_q });
	AddToggle(builder, Media::kHideGreetingSticker,
		tr::lng_nagram_hide_greeting_sticker(),
		u"nagram/media/hide-greeting-sticker"_q,
		{ u"greeting"_q, u"sticker"_q });
	AddToggle(builder, Media::kDisableVideoAutoplay,
		tr::lng_nagram_disable_video_autoplay(),
		u"nagram/media/disable-video-autoplay"_q,
		{ u"video"_q, u"autoplay"_q });
	AddToggle(builder, Media::kGifPlaybackControls,
		tr::lng_nagram_gif_playback_controls(),
		u"nagram/media/gif-playback-controls"_q,
		{ u"GIF"_q, u"playback"_q, u"controls"_q });
	AddToggle(builder, Media::kMp4FilePreview,
		tr::lng_nagram_mp4_file_preview(),
		u"nagram/media/mp4-file-preview"_q,
		{ u"MP4"_q, u"file"_q, u"preview"_q });
	builder.addButton({
		.id = u"nagram/media/sticker-catalog"_q,
		.title = tr::lng_nagram_catalog_title(),
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

} // namespace Nagram
