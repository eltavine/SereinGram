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

const auto kMeta = BuildHelper({
	.id = MediaSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_media,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	Media::AddLayout(builder);
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
