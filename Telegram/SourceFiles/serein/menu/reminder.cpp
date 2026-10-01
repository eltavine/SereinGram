#include "serein/menu/reminder.h"

#include "serein/hooks/menu/actions.h"
#include "api/api_common.h"
#include "apiwrap.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_schedule_box.h"
#include "history/view/history_view_scheduled_section.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "menu/menu_send_details.h"
#include "ui/text/text_utilities.h"
#include "ui/toast/toast.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_chat_helpers.h"
#include "styles/style_menu_icons.h"

namespace Serein::Menu {
namespace {

constexpr auto kActionIdProperty = "sereinMenuActionId";
constexpr auto kScheduledToastDuration = 4 * crl::time(1000);

[[nodiscard]] bool Available(not_null<HistoryItem*> item) {
	return item->isRegular()
		&& !item->isService()
		&& !item->forbidsForward()
		&& item->history()->peer->allowsForwarding();
}

void ShowScheduled(const std::shared_ptr<ChatHelpers::Show> &show) {
	if (!show->valid()) {
		return;
	}
	const auto session = &show->session();
	const auto filter = [=](const ClickHandlerPtr &, Qt::MouseButton) {
		if (const auto window = show->resolveWindow()) {
			window->showSection(
				std::make_shared<HistoryView::ScheduledMemento>(
					session->data().history(session->user())));
		}
		return false;
	};
	show->showToast({
		.text = tr::lng_reminder_scheduled_in(
			tr::now,
			lt_link,
			tr::link(tr::bold(tr::lng_saved_messages(tr::now))),
			tr::marked),
		.filter = filter,
		.iconLottie = u"toast/saved_messages"_q,
		.iconPadding = st::selfForwardsTaggerIconPadding,
		.st = &st::selfForwardsTaggerToast,
		.attach = RectPart::Top,
		.duration = kScheduledToastDuration,
	});
}

void Schedule(
		not_null<Main::Session*> session,
		std::shared_ptr<ChatHelpers::Show> show,
		FullMsgId itemId,
		Api::SendOptions options) {
	const auto item = session->data().message(itemId);
	if (!item || !Available(item)) {
		return;
	}
	auto resolved = item->history()->resolveForwardDraft(Data::ForwardDraft{
		.ids = session->data().itemOrItsGroup(item),
		.options = Data::ForwardOptions::PreserveInfo,
	});
	if (resolved.items.empty()) {
		return;
	}
	auto action = Api::SendAction(
		session->data().history(session->user()),
		options);
	action.clearDraft = false;
	action.generateLocal = false;
	session->api().forwardMessages(std::move(resolved), action, [=] {
		ShowScheduled(show);
	});
}

void ChooseTime(
		not_null<Window::SessionController*> controller,
		FullMsgId itemId) {
	const auto session = &controller->session();
	const auto show = controller->uiShow();
	controller->show(HistoryView::PrepareScheduleBox(
		session,
		show,
		SendMenu::Details{ .type = SendMenu::Type::Reminder },
		[=](Api::SendOptions options) {
			Schedule(session, show, itemId, options);
		}));
}

[[nodiscard]] int InsertPosition(not_null<Ui::PopupMenu*> menu) {
	const auto &actions = menu->actions();
	auto afterRepeat = -1;
	auto beforeDelete = -1;
	for (auto index = 0; index != int(actions.size()); ++index) {
		const auto value = actions[index]->property(kActionIdProperty);
		if (!value.isValid()) {
			continue;
		}
		const auto id = ActionId(value.toInt());
		if (id == ActionId::Repeat
			|| id == ActionId::RepeatAsCopy
			|| id == ActionId::ForwardWithoutQuote) {
			afterRepeat = index + 1;
		} else if (id == ActionId::Delete && beforeDelete < 0) {
			beforeDelete = index;
		}
	}
	return (afterRepeat >= 0)
		? afterRepeat
		: (beforeDelete >= 0)
		? beforeDelete
		: int(actions.size());
}

} // namespace

void InsertReminderAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !Available(item)) {
		return;
	}
	const auto itemId = item->fullId();
	const auto action = Ui::Menu::CreateAction(
		menu,
		tr::lng_context_set_reminder(tr::now),
		crl::guard(controller, [=] { ChooseTime(controller, itemId); }));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(),
		menu->menu()->st(),
		action,
		&st::menuIconNotifications,
		&st::menuIconNotifications);
	Tag(
		menu->insertAction(InsertPosition(menu), std::move(widget)),
		ActionId::Reminder);
}

} // namespace Serein::Menu
