#include "nagram/settings/messages.h"

#include "nagram/core/options.h"
#include "nagram/messages/options.h"
#include "nagram/settings/home.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_layers.h"
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

Ui::SettingsButton *AddToggle(
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
	return button;
}

void AddReactionChild(
		SectionBuilder &builder,
		const Option<bool> &option,
		rpl::producer<QString> title,
		QString id,
		QStringList keywords) {
	if (const auto button = AddToggle(builder, option,
			std::move(title), std::move(id), std::move(keywords))) {
		ForDevice().Value(Messages::kHideReactions
		) | rpl::on_next([button](bool hidden) {
			button->setDisabled(hidden);
			button->setEnabled(!hidden);
		}, button->lifetime());
	}
}

void EditMarkBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_nagram_edited_mark());
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
	builder.addSubsectionTitle({
		.id = u"nagram/messages/marks"_q,
		.title = tr::lng_nagram_marks_and_counts(),
		.keywords = { u"labels"_q, u"counts"_q },
	});
	AddToggle(builder, Messages::kExactMessageCounters,
		tr::lng_nagram_exact_message_counters(),
		u"nagram/messages/exact-counters"_q,
		{ u"exact"_q, u"views"_q, u"replies"_q });
	AddToggle(builder, Messages::kHideMessageViews,
		tr::lng_nagram_hide_message_views(),
		u"nagram/messages/hide-views"_q,
		{ u"hide"_q, u"views"_q });
	AddToggle(builder, Messages::kHideChannelSignature,
		tr::lng_nagram_hide_channel_signature(),
		u"nagram/messages/hide-signature"_q,
		{ u"channel"_q, u"signature"_q });
	AddToggle(builder, Messages::kHideEditedBadge,
		tr::lng_nagram_hide_edited_badge(),
		u"nagram/messages/hide-edited"_q,
		{ u"edited"_q, u"badge"_q });
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"nagram/messages/edited-mark"_q,
		.title = tr::lng_nagram_edited_mark(),
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
	builder.addSubsectionTitle({
		.id = u"nagram/messages/reactions"_q,
		.title = tr::lng_nagram_reactions(),
		.keywords = { u"reactions"_q },
	});
	AddToggle(builder, Messages::kHideReactions,
		tr::lng_nagram_hide_reactions(),
		u"nagram/messages/hide-reactions"_q,
		{ u"hide"_q, u"reactions"_q });
	AddReactionChild(builder, Messages::kHidePrivateReactions,
		tr::lng_nagram_hide_private_reactions(),
		u"nagram/messages/reactions-private"_q,
		{ u"private"_q, u"chat reactions"_q });
	AddReactionChild(builder, Messages::kHideGroupReactions,
		tr::lng_nagram_hide_group_reactions(),
		u"nagram/messages/reactions-group"_q,
		{ u"group"_q, u"chat reactions"_q });
	AddReactionChild(builder, Messages::kHideChannelReactions,
		tr::lng_nagram_hide_channel_reactions(),
		u"nagram/messages/reactions-channel"_q,
		{ u"channel"_q, u"chat reactions"_q });
	AddToggle(builder, Messages::kHideReactionMenu,
		tr::lng_nagram_hide_reaction_menu(),
		u"nagram/messages/reaction-menu"_q,
		{ u"context menu"_q, u"reaction panel"_q });
	AddToggle(builder, Messages::kHideReactionMenuWhenSelecting,
		tr::lng_nagram_hide_reaction_menu_when_selecting(),
		u"nagram/messages/reaction-selection"_q,
		{ u"selection"_q, u"reaction panel"_q });
	builder.addSubsectionTitle({
		.id = u"nagram/messages/effects"_q,
		.title = tr::lng_nagram_effects(),
		.keywords = { u"effects"_q, u"animation"_q },
	});
	AddToggle(builder, Messages::kDisablePremiumStickerEffects,
		tr::lng_nagram_disable_premium_sticker_effects(),
		u"nagram/messages/effects-premium"_q,
		{ u"Premium"_q, u"sticker effects"_q });
	AddToggle(builder, Messages::kDisableEmojiInteractions,
		tr::lng_nagram_disable_emoji_interactions(),
		u"nagram/messages/effects-emoji"_q,
		{ u"emoji"_q, u"interactions"_q });
	AddToggle(builder, Messages::kDisableMessageEffects,
		tr::lng_nagram_disable_message_effects(),
		u"nagram/messages/effects-message"_q,
		{ u"message effects"_q });
});

const SectionBuildMethod MessagesSection::kBuild = kMeta.build;

} // namespace

Settings::Type MessagesId() {
	return MessagesSection::Id();
}

} // namespace Nagram
