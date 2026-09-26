#include "nagram/settings/messages.h"

#include "nagram/core/options.h"
#include "nagram/messages/options.h"
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
		return tr::lng_nagram_messages();
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
	.id = MessagesSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_nagram_messages,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	builder.addSubsectionTitle({
		.id = u"nagram/messages/time"_q,
		.title = tr::lng_nagram_time_and_info(),
		.keywords = { u"time"_q, u"info"_q },
	});
	AddToggle(builder, Messages::kSecondsInMessages,
		tr::lng_nagram_seconds_in_messages(),
		u"nagram/messages/seconds"_q,
		{ u"seconds"_q, u"timestamp"_q });
	AddToggle(builder, Messages::kShowForwardedMessageDate,
		tr::lng_nagram_show_forwarded_message_date(),
		u"nagram/messages/forwarded-date"_q,
		{ u"forwarded"_q, u"original time"_q });
	AddToggle(builder, Messages::kShowServiceTime,
		tr::lng_nagram_show_service_time(),
		u"nagram/messages/service-time"_q,
		{ u"service"_q, u"time"_q });
	AddToggle(builder, Messages::kShowMessageId,
		tr::lng_nagram_show_message_id(),
		u"nagram/messages/id"_q,
		{ u"message ID"_q, u"tooltip"_q });
});

const SectionBuildMethod MessagesSection::kBuild = kMeta.build;

} // namespace

Settings::Type MessagesId() {
	return MessagesSection::Id();
}

} // namespace Nagram
