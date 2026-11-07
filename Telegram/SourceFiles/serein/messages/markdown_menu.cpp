#include "serein/messages/markdown_menu.h"

#include "serein/hooks/gen/privacy.h"
#include "serein/hooks/menu/actions.h"
#include "serein/messages/markdown.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>

namespace Serein::Messages {

void InsertCopyMarkdownAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || item->isService()) {
		return;
	} else if (!Hooks::Privacy::SaveProtectedContent()
		&& (!item->history()->peer->allowsForwarding()
			|| item->forbidsForward())) {
		return;
	}
	const auto text = item->translatedText();
	if (text.text.isEmpty()) {
		return;
	}
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_menu_copy_markdown(tr::now),
		crl::guard(controller, [=] {
			QGuiApplication::clipboard()->setText(ToMarkdown(text));
			controller->showToast(tr::lng_serein_markdown_copied(tr::now));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconCopy, &st::menuIconCopy);
	Menu::Tag(
		menu->insertAction(Menu::DeleteActionIndex(menu), std::move(widget)),
		Menu::ActionId::CopyMarkdown);
}

} // namespace Serein::Messages
