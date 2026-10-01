#include "serein/settings/chats.h"

#include "serein/chats/options.h"
#include "serein/hooks/chats/sort.h"
#include "serein/core/options.h"
#include "serein/settings/gen/chats_rows.h"
#include "serein/settings/home.h"
#include "serein/settings/lock.h"
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
#include "ui/widgets/labels.h"
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
		Serein::GuardSettings(content, [=] { build(content, kBuild); });
		Ui::ResizeFitChild(this, content);
	}

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_chats();
	}

	static const SectionBuildMethod kBuild;
};

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

[[nodiscard]] QString HiddenFoldersLabel(not_null<Main::Session*> session) {
	const auto value = ForAccount(session).Get(Chats::kHiddenFolderIds);
	const auto count = value.split(u',', Qt::SkipEmptyParts).size();
	return count
		? QString::number(count)
		: tr::lng_serein_config_off(tr::now);
}

void HiddenFoldersBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	box->setTitle(tr::lng_serein_hidden_folders());
	const auto current = ForAccount(session).Get(Chats::kHiddenFolderIds)
		.split(u',', Qt::SkipEmptyParts);
	auto checks = std::vector<std::pair<FilterId, Ui::Checkbox*>>();
	for (const auto &filter : session->data().chatsFilters().list()) {
		if (!filter.id()) {
			continue;
		}
		const auto check = box->addRow(object_ptr<Ui::Checkbox>(
			box,
			filter.title().text.text,
			current.contains(QString::number(filter.id()))));
		checks.emplace_back(filter.id(), check);
	}
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		(checks.empty()
			? tr::lng_serein_hidden_folders_empty()
			: tr::lng_serein_hidden_folders_about()),
		st::boxDividerLabel));
	if (checks.empty()) {
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	box->addButton(tr::lng_settings_save(), [=] {
		auto ids = std::vector<FilterId>();
		for (const auto &[id, check] : checks) {
			if (check->checked()) {
				ids.push_back(id);
			}
		}
		ranges::sort(ids);
		auto parts = QStringList();
		for (const auto id : ids) {
			parts.push_back(QString::number(id));
		}
		Expects(ForAccount(session).Set(
			Chats::kHiddenFolderIds,
			parts.join(u',')));
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

const auto kMeta = BuildHelper({
	.id = ChatsSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_chats,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	Chats::AddLayout(builder, {
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
		.hiddenFolderIds = [&] {
			const auto session = &controller->session();
			builder.addButton({
				.id = u"serein/chats/hidden-folders"_q,
				.title = tr::lng_serein_hidden_folders(),
				.st = &st::settingsButtonNoIcon,
				.label = ForAccount(session).Value(
					Chats::kHiddenFolderIds
				) | rpl::map([=](const QString &) {
					return HiddenFoldersLabel(session);
				}),
				.onClick = [=] {
					controller->show(Box(HiddenFoldersBox, session));
				},
				.keywords = { u"folder"_q, u"hide"_q, u"tabs"_q },
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
