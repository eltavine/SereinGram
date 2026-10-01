#include "serein/settings/home.h"
#include "serein/settings/interface.h"
#include "serein/settings/messages.h"
#include "serein/settings/menu.h"
#include "serein/settings/chats.h"
#include "serein/settings/compose.h"
#include "serein/settings/config.h"
#include "serein/settings/media.h"
#include "serein/settings/privacy.h"
#include "serein/settings/services.h"
#include "serein/settings/rules.h"
#include "serein/settings/lock.h"

#include "boxes/about_box.h"
#include "core/click_handler_types.h"
#include "lang/lang_keys.h"
#include "settings/sections/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class Home final : public Section<Home> {
public:
	Home(QWidget *parent, not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		Serein::GuardSettings(content, [=] { build(content, kBuild); });
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_settings();
	}

	static const SectionBuildMethod kBuild;
};

const auto kMeta = BuildHelper({
	.id = Home::Id(),
	.parentId = MainId(),
	.title = &tr::lng_serein_settings,
	.icon = &st::menuIconSerein,
}, [](SectionBuilder &builder) {
	builder.addButton({
		.title = tr::lng_serein_settings(),
		.icon = { &st::menuIconSerein },
		.label = rpl::single(tr::lng_settings_current_version(
			tr::now, lt_version, currentVersionShortText())),
	});
	builder.addButton({
		.title = tr::lng_serein_source_code(),
		.icon = { &st::menuIconLink },
		.onClick = [] {
			UrlClickHandler::Open(u"https://github.com/eltavine/SereinGram"_q);
		},
	});
	builder.addSectionButton({
		.title = tr::lng_serein_interface(),
		.targetSection = InterfaceId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"interface"_q, u"appearance"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_messages(),
		.targetSection = MessagesId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"messages"_q, u"time"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_chats(),
		.targetSection = ChatsId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"chats"_q, u"list"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_compose(),
		.targetSection = ComposeId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"compose"_q, u"send"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_media(),
		.targetSection = MediaId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"media"_q, u"sticker"_q, u"emoji"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_menu(),
		.targetSection = MenuId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"menu"_q, u"actions"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_privacy(),
		.targetSection = PrivacyId(),
		.icon = { &st::menuIconLock },
		.keywords = { u"privacy"_q, u"phone"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_services(),
		.targetSection = ServicesId(),
		.icon = { &st::menuIconTranslate },
		.keywords = { u"translation"_q, u"AI"_q, u"service"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_rules(),
		.targetSection = RulesId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"filter"_q, u"link"_q },
	});
	builder.addSectionButton({
		.title = tr::lng_serein_config_title(),
		.targetSection = ConfigId(),
		.icon = { &st::menuIconChatBubble },
		.keywords = { u"backup"_q, u"import"_q, u"export"_q },
	});
	builder.addDividerText(tr::lng_serein_settings_note());
});

const SectionBuildMethod Home::kBuild = kMeta.build;

} // namespace

Settings::Type HomeId() {
	return Home::Id();
}

void AddSettingsEntry(SectionBuilder &builder) {
	builder.addSectionButton({
		.title = tr::lng_serein_settings(),
		.targetSection = Home::Id(),
		.icon = { &st::menuIconSerein },
		.keywords = { u"Serein"_q, u"desktop"_q },
	});
}

} // namespace Serein
