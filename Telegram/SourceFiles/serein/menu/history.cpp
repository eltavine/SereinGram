#include "serein/menu/history.h"

#include "serein/hooks/history.h"
#include "serein/menu/actions.h"
#include "serein/ports/history_store.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

namespace Serein::Menu {
namespace {

constexpr auto kDeletedLimit = 100;

[[nodiscard]] std::vector<History::Record> EditVersions(
		not_null<HistoryItem*> item) {
	const auto store = Hooks::HistoryStoreFor(&item->history()->session());
	if (!store) {
		return {};
	}
	auto result = store->versions(
		qint64(item->history()->peer->id.value),
		item->id.bare);
	std::erase_if(result, [](const History::Record &record) {
		return record.kind != History::RecordKind::Edited;
	});
	return result;
}

[[nodiscard]] std::vector<History::Record> DeletedMessages(
		not_null<HistoryItem*> item) {
	const auto store = Hooks::HistoryStoreFor(&item->history()->session());
	if (!store) {
		return {};
	}
	return store->deleted({
		.peerId = qint64(item->history()->peer->id.value),
		.limit = kDeletedLimit,
	});
}

void ShowRecords(
		not_null<Window::SessionController*> controller,
		const QString &title,
		const QString &text) {
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(rpl::single(title));
		const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
			box, text, st::boxLabel));
		label->setSelectable(true);
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

[[nodiscard]] QString Describe(const std::vector<History::Record> &versions) {
	auto blocks = QStringList();
	for (const auto &version : versions) {
		const auto when = base::unixtime::parse(TimeId(version.recordedAt));
		const auto body = version.text.isEmpty()
			? version.mediaSummary
			: version.text;
		blocks.push_back(langDateTimeFull(when) + u"\n"_q + body);
	}
	return blocks.join(u"\n\n"_q);
}

} // namespace

void InsertEditHistoryAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || EditVersions(item).empty()) {
		return;
	}
	const auto itemId = item->fullId();
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_menu_edit_history(tr::now),
		crl::guard(controller, [=] {
			const auto current = controller->session().data().message(itemId);
			if (!current) {
				return;
			}
			ShowRecords(
				controller,
				tr::lng_serein_menu_edit_history(tr::now),
				Describe(EditVersions(current)));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconInfo, &st::menuIconInfo);
	Tag(menu->insertAction(DeleteActionIndex(menu), std::move(widget)),
		ActionId::EditHistory);
}

void InsertDeletedMessagesAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || DeletedMessages(item).empty()) {
		return;
	}
	const auto itemId = item->fullId();
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_menu_deleted_messages(tr::now),
		crl::guard(controller, [=] {
			const auto current = controller->session().data().message(itemId);
			if (!current) {
				return;
			}
			ShowRecords(
				controller,
				tr::lng_serein_menu_deleted_messages(tr::now),
				Describe(DeletedMessages(current)));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconInfo, &st::menuIconInfo);
	Tag(menu->insertAction(DeleteActionIndex(menu), std::move(widget)),
		ActionId::DeletedMessages);
}

} // namespace Serein::Menu
