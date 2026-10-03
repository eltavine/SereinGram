#include "serein/settings/menu.h"

#include "serein/menu/model.h"
#include "serein/settings/home.h"
#include "serein/settings/gen/menu_rows.h"
#include "serein/hooks/core/language.h"
#include "serein/settings/page.h"
#include "lang/lang_instance.h"
#include "lang/lang_keys.h"
#include "lang_auto_counts.h"
#include "settings/settings_builder.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/checkbox.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"
#include "styles/style_settings.h"
#include "styles/style_layers.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class MenuSection final : public Page<MenuSection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_menu();
	}

	static const SectionBuildMethod kBuild;

};

QString Title(Menu::ActionId id) {
	for (const auto &entry : Menu::kEntries) {
		if (entry.id == id) {
			const auto key = QLatin1String(entry.titleKey);
			const auto index = Lang::GetKeyIndex(key);
			return (index == Lang::kKeysCount)
				? QString(key)
				: LocalizedValue(Lang::GetInstance(), index);
		}
	}
	return QString();
}

QString VisibilityLabel(Menu::Visibility visibility) {
	switch (visibility) {
	case Menu::Visibility::Hide: return tr::lng_serein_menu_hidden(tr::now);
	case Menu::Visibility::WithOption: return tr::lng_serein_menu_with_option(tr::now);
	default: return tr::lng_serein_menu_shown(tr::now);
	}
}

const style::icon *Icon(Menu::ActionId id) {
	switch (id) {
	case Menu::ActionId::Reply: return &st::menuIconReply;
	case Menu::ActionId::Edit: return &st::menuIconEdit;
	case Menu::ActionId::Copy: return &st::menuIconCopy;
	case Menu::ActionId::CopyLink: return &st::menuIconLink;
	case Menu::ActionId::Forward: return &st::menuIconForward;
	case Menu::ActionId::Translate: return &st::menuIconTranslate;
	case Menu::ActionId::Pin: return &st::menuIconPin;
	case Menu::ActionId::Select: return &st::menuIconSelect;
	case Menu::ActionId::Statistics: return &st::menuIconStats;
	case Menu::ActionId::Report: return &st::menuIconReport;
	case Menu::ActionId::BlockSender: return &st::menuIconBlock;
	case Menu::ActionId::Image: return &st::menuIconSaveImage;
	case Menu::ActionId::Delete: return &st::menuIconDelete;
	case Menu::ActionId::StickerPack: return &st::menuIconStickers;
	case Menu::ActionId::Repeat:
	case Menu::ActionId::RepeatAsCopy: return &st::menuIconRepeat;
	case Menu::ActionId::ForwardWithoutQuote: return &st::menuIconForward;
	case Menu::ActionId::Batch: return &st::menuIconCopy;
	case Menu::ActionId::SelectSender: return &st::menuIconSelect;
	case Menu::ActionId::MediaInfo: return &st::menuIconInfo;
	case Menu::ActionId::Screenshot: return &st::menuIconSaveImage;
	case Menu::ActionId::Reading: return &st::menuIconTranslate;
	case Menu::ActionId::FilterAuthor: return &st::menuIconBlock;
	case Menu::ActionId::EditHistory: return &st::menuIconEdit;
	case Menu::ActionId::DeletedMessages: return &st::menuIconRestore;
	case Menu::ActionId::ReadUntilHere: return &st::menuIconMarkRead;
	case Menu::ActionId::HistoryExclusion: return &st::menuIconBlock;
	case Menu::ActionId::CopyMarkdown: return &st::menuIconCopy;
	case Menu::ActionId::ButtonData:
	case Menu::ActionId::MessageDetails: return &st::menuIconInfo;
	case Menu::ActionId::SelectRange: return &st::menuIconSelect;
	case Menu::ActionId::BatchUnpin: return &st::menuIconUnpin;
	case Menu::ActionId::QuickRatingFirst: return &st::menuIconLike;
	case Menu::ActionId::QuickRatingSecond: return &st::menuIconReply;
	case Menu::ActionId::Reminder: return &st::menuIconNotifications;
	case Menu::ActionId::HideMessage: return &st::menuIconCaptionHide;
	default: return &st::menuIconChatBubble;
	}
}

void VisibilityBox(not_null<Ui::GenericBox*> box, Menu::ActionId id) {
	box->setTitle(Title(id));
	const auto current = Menu::ReadVisibility(
		ForDevice().Get(Menu::kMenuConfig), id);
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(int(current));
	for (const auto state : { Menu::Visibility::Show,
			Menu::Visibility::Hide, Menu::Visibility::WithOption }) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, int(state), VisibilityLabel(state),
			st::settingsSendType), st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		const auto current = ForDevice().Get(Menu::kMenuConfig);
		Expects(ForDevice().Set(Menu::kMenuConfig, Menu::WriteVisibility(
			current, id, Menu::Visibility(value))));
		box->closeBox();
	});
}

const auto kMeta = BuildHelper({
	.id = MenuSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_menu,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	Menu::AddLayout(builder, {
		.messageMenu = [&] {
			for (const auto &entry : Menu::kEntries) {
				const auto id = entry.id;
				builder.addButton({
					.id = u"serein/menu/"_q
						+ QString::fromLatin1(entry.titleKey),
					.title = rpl::single(Title(id)),
					.icon = { Icon(id) },
					.label = ForDevice().Value(Menu::kMenuConfig)
						| rpl::map([=](const QByteArray &config) {
							return VisibilityLabel(
								Menu::ReadVisibility(config, id));
						}),
					.onClick = [=] {
						controller->show(Box(VisibilityBox, id));
					},
					.keywords = { Title(id) },
				});
			}
		},
	});
});

const SectionBuildMethod MenuSection::kBuild = kMeta.build;

} // namespace

Settings::Type MenuId() {
	return MenuSection::Id();
}

} // namespace Serein
