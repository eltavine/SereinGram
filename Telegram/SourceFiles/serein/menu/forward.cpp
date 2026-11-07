#include "serein/menu/forward.h"

#include "serein/hooks/menu/actions.h"
#include "serein/menu/model.h"

#include <QtGui/QGuiApplication>
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_peer_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

namespace Serein::Menu {
namespace {

void Forward(
		not_null<Window::SessionController*> controller,
		FullMsgId id,
		Data::ForwardOptions options,
		bool saved) {
	const auto item = controller->session().data().message(id);
	if (!item || !item->allowsForward() || item->isLocal()) {
		return;
	}
	auto &data = item->history()->owner();
	auto ids = data.itemOrItsGroup(item);
	if (ranges::any_of(ids, [&](FullMsgId id) {
		const auto item = data.message(id);
		return !item || item->isLocal() || !item->allowsForward();
	})) {
		return;
	}
	auto draft = Data::ForwardDraft{ .ids = std::move(ids), .options = options };
	if (saved) {
		Window::ForwardToSelf(controller->uiShow(), draft);
	} else {
		Window::ShowForwardMessagesBox(controller, std::move(draft));
	}
}

void Fill(
		not_null<Ui::PopupMenu*> submenu,
		not_null<Window::SessionController*> controller,
		FullMsgId id,
		bool saved) {
	for (const auto option : {
			Data::ForwardOptions::PreserveInfo,
			Data::ForwardOptions::NoSenderNames,
			Data::ForwardOptions::NoNamesAndCaptions }) {
		const auto title = (option == Data::ForwardOptions::PreserveInfo)
			? tr::lng_serein_forward_original(tr::now)
			: (option == Data::ForwardOptions::NoSenderNames)
			? tr::lng_serein_forward_no_sender(tr::now)
			: tr::lng_serein_forward_no_caption(tr::now);
		submenu->addAction(title, crl::guard(controller, [=] {
			Forward(controller, id, option, saved);
		}), &st::menuIconForward);
	}
}

} // namespace

void InsertForwardActions(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !item->allowsForward()
		|| item->isService() || item->isLocal()) {
		return;
	}
	const auto showOptions = Visible(ReadVisibility(ForDevice().Get(kMenuConfig),
		ActionId::ForwardWithoutQuote), QGuiApplication::keyboardModifiers() & Qt::AltModifier);
	for (const auto &action : menu->actions()) {
		if (showOptions && action->text() == tr::lng_context_forward_msg(tr::now)) {
			Fill(menu->ensureSubmenu(action, menu->st()), controller, item->fullId(), false);
			break;
		}
	}
	const auto action = menu->addAction(
		tr::lng_serein_forward_saved(tr::now),
		[] {},
		&st::menuIconSavedMessages);
	Tag(action, ActionId::ForwardToSaved);
	Fill(menu->ensureSubmenu(action, menu->st()), controller, item->fullId(), true);
}

} // namespace Serein::Menu
