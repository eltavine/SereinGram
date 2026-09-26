#include "nagram/settings/compose.h"

#include "nagram/compose/options.h"
#include "nagram/settings/home.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Nagram {
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
		return tr::lng_nagram_compose();
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

const auto kMeta = BuildHelper({
	.id = ComposeSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_nagram_compose,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	builder.addSubsectionTitle({
		.id = u"nagram/compose/buttons"_q,
		.title = tr::lng_nagram_compose_buttons(),
		.keywords = { u"buttons"_q, u"compose"_q },
	});
	AddToggle(builder, Compose::kHideAttachButton,
		tr::lng_nagram_hide_attach_button(),
		u"nagram/compose/hide-attach"_q, { u"attach"_q });
	AddToggle(builder, Compose::kHideEmojiButton,
		tr::lng_nagram_hide_emoji_button(),
		u"nagram/compose/hide-emoji"_q, { u"emoji"_q });
	AddToggle(builder, Compose::kHideRecordingButton,
		tr::lng_nagram_hide_recording_button(),
		u"nagram/compose/hide-recording"_q, { u"recording"_q });
	AddToggle(builder, Compose::kHideBotCommandButton,
		tr::lng_nagram_hide_bot_command_button(),
		u"nagram/compose/hide-bot-command"_q, { u"bot"_q, u"command"_q });
	AddToggle(builder, Compose::kHideBotMenu,
		tr::lng_nagram_hide_bot_menu(),
		u"nagram/compose/hide-bot-menu"_q, { u"bot"_q, u"menu"_q });
	AddToggle(builder, Compose::kHideAutoDeleteButton,
		tr::lng_nagram_hide_auto_delete_button(),
		u"nagram/compose/hide-auto-delete"_q, { u"delete"_q, u"timer"_q });
	AddToggle(builder, Compose::kHideGiftButton,
		tr::lng_nagram_hide_gift_button(),
		u"nagram/compose/hide-gift"_q, { u"gift"_q });
	AddToggle(builder, Compose::kHideAiButton,
		tr::lng_nagram_hide_ai_button(),
		u"nagram/compose/hide-ai"_q, { u"AI"_q });
	AddToggle(builder, Compose::kHideSendAsButton,
		tr::lng_nagram_hide_send_as_button(),
		u"nagram/compose/hide-send-as"_q, { u"send as"_q });
	AddToggle(builder, Compose::kHideStarsReactionButton,
		tr::lng_nagram_hide_stars_reaction_button(),
		u"nagram/compose/hide-stars-reaction"_q, { u"Stars"_q });
	AddToggle(builder, Compose::kHideChannelMuteButton,
		tr::lng_nagram_hide_channel_mute_button(),
		u"nagram/compose/hide-channel-mute"_q, { u"channel"_q, u"mute"_q });
});

const SectionBuildMethod ComposeSection::kBuild = kMeta.build;

} // namespace

Settings::Type ComposeId() {
	return ComposeSection::Id();
}

} // namespace Nagram
