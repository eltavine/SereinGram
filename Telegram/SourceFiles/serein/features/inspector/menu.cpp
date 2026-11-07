#include "serein/features/inspector/menu.h"

#include "serein/features/inspector/box.h"
#include "serein/hooks/menu/actions.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Inspector {

void InsertDetailsAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !item->isRegular()) {
		return;
	}
	const auto itemId = item->fullId();
	const auto action = Ui::Menu::CreateAction(
		menu,
		tr::lng_serein_menu_details(tr::now),
		crl::guard(controller, [=] {
			const auto current = controller->session().data().message(itemId);
			if (current) {
				ShowMessageDetails(controller, current);
			}
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(),
		menu->menu()->st(),
		action,
		&st::menuIconInfo,
		&st::menuIconInfo);
	Menu::Tag(
		menu->insertAction(Menu::DeleteActionIndex(menu), std::move(widget)),
		Menu::ActionId::MessageDetails);
}

} // namespace Serein::Inspector
