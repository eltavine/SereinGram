#include "nagram/settings/menu.h"

#include "nagram/menu/model.h"
#include "nagram/settings/home.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/checkbox.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_layers.h"

namespace Nagram {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class MenuSection final : public Section<MenuSection> {
public:
	MenuSection(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_nagram_menu();
	}

	static const SectionBuildMethod kBuild;
};

QString Title(Menu::ActionId id) {
	switch (id) {
	case Menu::ActionId::Reply: return tr::lng_nagram_menu_reply(tr::now);
	case Menu::ActionId::Edit: return tr::lng_nagram_menu_edit(tr::now);
	case Menu::ActionId::Copy: return tr::lng_nagram_menu_copy(tr::now);
	case Menu::ActionId::CopyLink: return tr::lng_nagram_menu_copy_link(tr::now);
	case Menu::ActionId::Forward: return tr::lng_nagram_menu_forward(tr::now);
	case Menu::ActionId::Translate: return tr::lng_nagram_menu_translate(tr::now);
	case Menu::ActionId::Pin: return tr::lng_nagram_menu_pin(tr::now);
	case Menu::ActionId::Select: return tr::lng_nagram_menu_select(tr::now);
	case Menu::ActionId::Statistics: return tr::lng_nagram_menu_statistics(tr::now);
	case Menu::ActionId::Report: return tr::lng_nagram_menu_report(tr::now);
	case Menu::ActionId::BlockSender: return tr::lng_nagram_menu_block_sender(tr::now);
	case Menu::ActionId::Image: return tr::lng_nagram_menu_image(tr::now);
	case Menu::ActionId::Delete: return tr::lng_nagram_menu_delete(tr::now);
	case Menu::ActionId::StickerPack: return tr::lng_nagram_menu_sticker_pack(tr::now);
	case Menu::ActionId::Repeat: return tr::lng_nagram_menu_repeat(tr::now);
	case Menu::ActionId::RepeatAsCopy: return tr::lng_nagram_menu_repeat_as_copy(tr::now);
	case Menu::ActionId::ForwardWithoutQuote: return tr::lng_nagram_menu_forward_without_quote(tr::now);
	default: return QString();
	}
}

QString VisibilityLabel(Menu::Visibility visibility) {
	switch (visibility) {
	case Menu::Visibility::Hide: return tr::lng_nagram_menu_hidden(tr::now);
	case Menu::Visibility::WithOption: return tr::lng_nagram_menu_with_option(tr::now);
	default: return tr::lng_nagram_menu_shown(tr::now);
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
	.title = &tr::lng_nagram_menu,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	const auto confirm = builder.addButton({
		.id = u"nagram/menu/confirm-repeat"_q,
		.title = tr::lng_nagram_menu_confirm_repeat(),
		.st = &st::settingsButtonNoIcon,
		.toggled = ForDevice().Value(Menu::kConfirmRepeat),
		.keywords = { u"repeat"_q, u"confirm"_q },
	});
	if (confirm) {
		confirm->toggledChanges(
		) | rpl::on_next([](bool value) {
			Expects(ForDevice().Set(Menu::kConfirmRepeat, value));
		}, confirm->lifetime());
	}
	for (const auto &entry : Menu::kEntries) {
		const auto id = entry.id;
		builder.addButton({
			.id = u"nagram/menu/"_q + QString::fromLatin1(entry.titleKey),
			.title = rpl::single(Title(id)),
			.icon = { Icon(id) },
			.label = ForDevice().Value(Menu::kMenuConfig)
				| rpl::map([=](const QByteArray &config) {
					return VisibilityLabel(Menu::ReadVisibility(config, id));
				}),
			.onClick = [=] { controller->show(Box(VisibilityBox, id)); },
			.keywords = { Title(id) },
		});
	}
	builder.addDividerText(tr::lng_nagram_menu_note());
});

const SectionBuildMethod MenuSection::kBuild = kMeta.build;

} // namespace

Settings::Type MenuId() {
	return MenuSection::Id();
}

} // namespace Nagram
