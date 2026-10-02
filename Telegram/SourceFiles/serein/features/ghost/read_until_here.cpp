#include "serein/features/ghost/read_until_here.h"

#include "serein/features/ghost/model/policy.h"
#include "serein/hooks/ghost.h"
#include "serein/hooks/menu/actions.h"
#include "data/data_histories.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Ghost {

void InsertReadUntilHereAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !item->isRegular() || item->out()) {
		return;
	}
	const auto session = &controller->session();
	const auto policy = Ghost::Read(ForAccount(session), ForDevice());
	if (Ghost::Allows(policy, Ghost::Activity::ReadReceipt)) {
		return;
	}
	const auto itemId = item->fullId();
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_menu_read_until_here(tr::now),
		crl::guard(controller, [=] {
			if (const auto current = session->data().message(itemId)) {
				const auto forced = Hooks::ForcedReadReceipt();
				session->data().histories().readInboxTill(current);
			}
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconMarkRead, &st::menuIconMarkRead);
	Menu::Tag(
		menu->insertAction(Menu::DeleteActionIndex(menu), std::move(widget)),
		Menu::ActionId::ReadUntilHere);
}

} // namespace Serein::Ghost
