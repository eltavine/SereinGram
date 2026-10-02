#include "serein/messages/reading_menu.h"

#include "serein/hooks/menu/actions.h"
#include "serein/messages/options.h"
#include "serein/hooks/messages/reading.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Messages {

void InsertReadingAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!Messages::ChineseConversionAvailable() || !menu || !item || !controller
		|| item->isService() || item->originalText().empty()) {
		return;
	}
	const auto spacing = ForDevice().Get(Messages::kReadingSpacing);
	const auto chinese = ForDevice().Get(Messages::kReadingChinese);
	if (!spacing && !chinese) {
		return;
	}
	const auto &original = item->translatedTextWithLocalEntities();
	const auto projected = Messages::ProjectReading(original, spacing, chinese);
	if (!projected || projected->text == original.text
		|| item->translatedRichPage()) {
		return;
	}
	const auto id = item->fullId();
	const auto session = &item->history()->owner();
	const auto title = item->sereinOriginalShown()
		? tr::lng_serein_reading_apply(tr::now)
		: tr::lng_serein_reading_original(tr::now);
	const auto action = Ui::Menu::CreateAction(menu, title,
		crl::guard(controller, [=] {
			if (const auto current = session->message(id)) {
				current->sereinToggleOriginalShown();
			}
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconTranslate, &st::menuIconTranslate);
	Menu::Tag(
		menu->insertAction(Menu::DeleteActionIndex(menu), std::move(widget)),
		Menu::ActionId::Reading);
}

} // namespace Serein::Messages
