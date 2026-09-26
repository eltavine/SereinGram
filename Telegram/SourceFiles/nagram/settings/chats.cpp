#include "nagram/settings/chats.h"

#include "nagram/chats/options.h"
#include "nagram/core/options.h"
#include "nagram/settings/home.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
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

class ChatsSection final : public Section<ChatsSection> {
public:
	ChatsSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller)
	: Section(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		build(content, kBuild);
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_nagram_chats();
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

QString PreviewLinesLabel(int value) {
	switch (value) {
	case 1: return tr::lng_nagram_preview_one(tr::now);
	case 2: return tr::lng_nagram_preview_two(tr::now);
	case 3: return tr::lng_nagram_preview_three(tr::now);
	default: return tr::lng_nagram_preview_follow(tr::now);
	}
}

void PreviewLinesBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_nagram_chat_preview_lines());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		ForDevice().Get(Chats::kPreviewLines));
	for (auto value = 0; value != 4; ++value) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value, PreviewLinesLabel(value), st::settingsSendType),
			st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		Expects(ForDevice().Set(Chats::kPreviewLines, value));
		box->closeBox();
	});
}

const auto kMeta = BuildHelper({
	.id = ChatsSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_nagram_chats,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	builder.addSubsectionTitle({
		.id = u"nagram/chats/list"_q,
		.title = tr::lng_nagram_chat_list(),
		.keywords = { u"list"_q, u"layout"_q },
	});
	AddToggle(builder, Chats::kCompactList,
		tr::lng_nagram_compact_chat_list(),
		u"nagram/chats/compact"_q,
		{ u"compact"_q, u"list"_q });
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"nagram/chats/preview-lines"_q,
		.title = tr::lng_nagram_chat_preview_lines(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(Chats::kPreviewLines)
			| rpl::map(PreviewLinesLabel),
		.onClick = [=] { controller->show(Box(PreviewLinesBox)); },
		.keywords = { u"preview"_q, u"lines"_q },
	});
	AddToggle(builder, Chats::kHideSavedAndArchivedPreviews,
		tr::lng_nagram_hide_saved_and_archived_previews(),
		u"nagram/chats/hide-special-previews"_q,
		{ u"saved"_q, u"archive"_q, u"preview"_q });
	AddToggle(builder, Chats::kHideStories,
		tr::lng_nagram_hide_stories(),
		u"nagram/chats/hide-stories"_q,
		{ u"stories"_q });
});

const SectionBuildMethod ChatsSection::kBuild = kMeta.build;

} // namespace

Settings::Type ChatsId() {
	return ChatsSection::Id();
}

} // namespace Nagram
