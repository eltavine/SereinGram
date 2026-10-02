#include "serein/menu/repeat.h"

#include "serein/hooks/menu/actions.h"
#include "serein/menu/model.h"
#include "api/api_common.h"
#include "apiwrap.h"
#include "data/data_chat_participant_status.h"
#include "data/data_forum_topic.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_peer_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

namespace Serein::Menu {
namespace {

constexpr auto kActionIdProperty = "sereinMenuActionId";

[[nodiscard]] bool Forwardable(HistoryItem *item) {
	return item
		&& item->allowsForward()
		&& !item->isService()
		&& !item->isLocal()
		&& item->id > 0;
}

[[nodiscard]] not_null<Data::Thread*> Thread(not_null<HistoryItem*> item) {
	return item->topic()
		? static_cast<Data::Thread*>(item->topic())
		: static_cast<Data::Thread*>(item->history());
}

[[nodiscard]] bool Repeatable(HistoryItem *item) {
	return Forwardable(item) && Data::CanSendAnything(Thread(item));
}

void Repeat(
		not_null<Window::SessionController*> controller,
		FullMsgId itemId,
		Data::ForwardOptions options) {
	const auto item = controller->session().data().message(itemId);
	if (!Repeatable(item)) {
		return;
	}
	const auto history = item->history();
	auto resolved = history->resolveForwardDraft({
		.ids = history->owner().itemOrItsGroup(item),
		.options = options,
	});
	if (resolved.items.empty()) {
		return;
	}
	auto action = Api::SendAction(Thread(item));
	action.clearDraft = false;
	action.generateLocal = false;
	history->session().api().forwardMessages(std::move(resolved), action);
}

void ForwardWithoutQuote(
		not_null<Window::SessionController*> controller,
		FullMsgId itemId) {
	const auto item = controller->session().data().message(itemId);
	if (!Forwardable(item)) {
		return;
	}
	Window::ShowForwardMessagesBox(controller, {
		.ids = item->history()->owner().itemOrItsGroup(item),
		.options = Data::ForwardOptions::NoSenderNames,
	});
}

void Run(
		not_null<Window::SessionController*> controller,
		FullMsgId itemId,
		ActionId id) {
	switch (id) {
	case ActionId::Repeat:
		Repeat(controller, itemId, Data::ForwardOptions::PreserveInfo);
		break;
	case ActionId::RepeatAsCopy:
		Repeat(controller, itemId, Data::ForwardOptions::NoSenderNames);
		break;
	default:
		ForwardWithoutQuote(controller, itemId);
		break;
	}
}

int InsertPosition(not_null<Ui::PopupMenu*> menu) {
	for (auto index = int(menu->actions().size()); index != 0;) {
		--index;
		const auto value = menu->actions()[index]->property(kActionIdProperty);
		if (value.isValid()
			&& value.toInt() == static_cast<int>(ActionId::Forward)) {
			return index + 1;
		}
	}
	return DeleteActionIndex(menu);
}

void Insert(
		not_null<Ui::PopupMenu*> menu,
		int position,
		not_null<Window::SessionController*> controller,
		FullMsgId itemId,
		ActionId id,
		const QString &text) {
	const auto callback = crl::guard(controller, [=] {
		if (id != ActionId::ForwardWithoutQuote
			&& ForDevice().Get(kConfirmRepeat)) {
			const auto weak = base::make_weak(controller);
			controller->show(Ui::MakeConfirmBox({
				.text = tr::lng_serein_menu_repeat_confirm_text(),
				.confirmed = [=](Fn<void()> &&close) {
					close();
					if (const auto strong = weak.get()) {
						Run(strong, itemId, id);
					}
				},
			}));
		} else {
			Run(controller, itemId, id);
		}
	});
	const auto action = Ui::Menu::CreateAction(menu, text, callback);
	const auto icon = (id == ActionId::ForwardWithoutQuote)
		? &st::menuIconForward : &st::menuIconRepeat;
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action, icon, icon);
	Tag(menu->insertAction(position, std::move(widget)), id);
}

} // namespace

void InsertRepeatActions(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!Forwardable(item) || !menu || !controller) {
		return;
	}
	const auto itemId = item->fullId();
	auto position = InsertPosition(menu);
	if (Repeatable(item)) {
		Insert(menu, position++, controller, itemId, ActionId::Repeat,
			tr::lng_serein_menu_repeat(tr::now));
		Insert(menu, position++, controller, itemId, ActionId::RepeatAsCopy,
			tr::lng_serein_menu_repeat_as_copy(tr::now));
	}
	Insert(menu, position, controller, itemId, ActionId::ForwardWithoutQuote,
		tr::lng_serein_menu_forward_without_quote(tr::now));
}

} // namespace Serein::Menu
