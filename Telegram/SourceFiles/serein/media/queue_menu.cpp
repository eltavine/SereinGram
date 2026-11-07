#include "serein/media/queue_menu.h"

#include "serein/hooks/media/download_queue.h"
#include "serein/hooks/menu/actions.h"
#include "serein/media/options.h"
#include "data/data_media_types.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

namespace Serein::Media {

void InsertQueueAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller,
		MessageIdsList selected) {
	if (!menu || !controller || !ForDevice().Get(kSequentialDownloads)) return;
	if (selected.empty() && item && item->media() && !item->forbidsSaving()) {
		selected = item->history()->owner().itemOrItsGroup(item);
	}
	if (selected.empty()) return;
	Menu::Tag(menu->addAction(tr::lng_serein_queue_add(tr::now),
		crl::guard(controller, [=] {
			controller->session().sereinDownloads().enqueue(selected);
		}), &st::menuIconDownload), Menu::ActionId::DownloadQueue);
}

} // namespace Serein::Media
