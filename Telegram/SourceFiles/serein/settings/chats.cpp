#include "serein/settings/chats.h"

#include "serein/chats/options.h"
#include "serein/chats/sort.h"
#include "serein/core/options.h"
#include "serein/settings/gen/chats_rows.h"
#include "serein/settings/home.h"
#include "data/data_chat_filters.h"
#include "data/data_session.h"
#include "main/main_session.h"
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

#include <bit>

namespace Serein {
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
		return tr::lng_serein_chats();
	}

	static const SectionBuildMethod kBuild;
};

QString PreviewLinesLabel(int value) {
	switch (value) {
	case 1: return tr::lng_serein_preview_one(tr::now);
	case 2: return tr::lng_serein_preview_two(tr::now);
	case 3: return tr::lng_serein_preview_three(tr::now);
	default: return tr::lng_serein_preview_follow(tr::now);
	}
}

void PreviewLinesBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_chat_preview_lines());
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

QString StartupFolderLabel(not_null<Main::Session*> session) {
	auto &options = ForAccount(session);
	switch (options.Get(Chats::kStartupFolderMode)) {
	case 1: return tr::lng_serein_startup_folder_last(tr::now);
	case 2: {
		const auto id = options.Get(Chats::kStartupFolderId);
		const auto &list = session->data().chatsFilters().list();
		const auto found = ranges::find(list, id, &Data::ChatFilter::id);
		return found == list.end()
			? tr::lng_serein_preview_follow(tr::now)
			: found->titleText().text;
	}
	default: return tr::lng_serein_preview_follow(tr::now);
	}
}

void StartupFolderBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	box->setTitle(tr::lng_serein_startup_folder());
	const auto options = &ForAccount(session);
	const auto mode = options->Get(Chats::kStartupFolderMode);
	const auto current = mode == 2
		? options->Get(Chats::kStartupFolderId) + 2
		: mode;
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(current);
	for (const auto value : { 0, 1 }) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, value,
			value ? tr::lng_serein_startup_folder_last(tr::now)
				: tr::lng_serein_preview_follow(tr::now),
			st::settingsSendType), st::settingsSendTypePadding);
	}
	for (const auto &filter : session->data().chatsFilters().list()) {
		if (!filter.id()) continue;
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, filter.id() + 2,
			tr::lng_serein_startup_folder_specific(tr::now)
				+ u" · "_q + filter.titleText().text,
			st::settingsSendType), st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int value) {
		if (value > 2) {
			Expects(options->Set(Chats::kStartupFolderId, value - 2));
			Expects(options->Set(Chats::kStartupFolderMode, 2));
		} else {
			Expects(options->Set(Chats::kStartupFolderMode, value));
		}
		box->closeBox();
	});
}

const auto kMeta = BuildHelper({
	.id = ChatsSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_chats,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	Chats::AddLayout(builder, {
		.chatPreviewLines = [&] {
			builder.addButton({
				.id = u"serein/chats/preview-lines"_q,
				.title = tr::lng_serein_chat_preview_lines(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Chats::kPreviewLines)
					| rpl::map(PreviewLinesLabel),
				.onClick = [=] { controller->show(Box(PreviewLinesBox)); },
				.keywords = { u"preview"_q, u"lines"_q },
			});
		},
		.startupFolderMode = [&] {
			const auto session = &controller->session();
			builder.addButton({
				.id = u"serein/chats/startup-folder"_q,
				.title = tr::lng_serein_startup_folder(),
				.st = &st::settingsButtonNoIcon,
				.label = rpl::single(StartupFolderLabel(session)) | rpl::then(
					ForAccount(session).changes() | rpl::map([=](auto) {
						return StartupFolderLabel(session);
					})),
				.onClick = [=] {
					controller->show(Box([=](not_null<Ui::GenericBox*> box) {
						StartupFolderBox(box, session);
					}));
				},
				.keywords = { u"startup"_q, u"folder"_q },
			});
		},
		.chatSort = [&] {
			builder.addButton({
				.id = u"serein/chats/chat-sort"_q,
				.title = tr::lng_serein_chat_sort(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(Chats::kChatSort)
					| rpl::map([](int value) {
						const auto count = std::popcount(unsigned(value & 15));
						return count
							? QString::number(count)
								+ tr::lng_serein_sort_active_suffix(tr::now)
							: tr::lng_serein_preview_follow(tr::now);
					}),
				.onClick = [=] { controller->show(Box(Chats::ChatSortBox)); },
				.keywords = { u"sort"_q, u"unread"_q, u"contacts"_q },
			});
		},
	});
});

const SectionBuildMethod ChatsSection::kBuild = kMeta.build;

} // namespace

Settings::Type ChatsId() {
	return ChatsSection::Id();
}

} // namespace Serein
