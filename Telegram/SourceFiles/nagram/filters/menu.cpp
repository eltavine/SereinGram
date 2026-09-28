#include "nagram/filters/menu.h"

#include "nagram/filters/model.h"
#include "nagram/menu/actions.h"
#include "data/data_peer.h"
#include "data/data_peer_id.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

namespace Nagram::Filters {

void InsertAuthorAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !item->from()) {
		return;
	}
	const auto session = &controller->session();
	const auto author = QString::number(SerializePeerId(item->from()->id));
	const auto raw = ForAccount(session).Get(kRules);
	if (!Validate(raw)) {
		return;
	}
	const auto config = raw.isEmpty()
		? Defaults() : QJsonDocument::fromJson(raw).object();
	const auto hidden = config.value(u"hiddenAuthors"_q).toArray().contains(author);
	const auto title = hidden
		? tr::lng_nagram_filter_author_show(tr::now)
		: tr::lng_nagram_filter_author_hide(tr::now);
	const auto action = Ui::Menu::CreateAction(menu, title,
		crl::guard(controller, [=] {
			const auto current = ForAccount(session).Get(kRules);
			if (!Validate(current)) {
				controller->showToast(tr::lng_nagram_filter_invalid(tr::now));
				return;
			}
			auto updated = current.isEmpty()
				? Defaults() : QJsonDocument::fromJson(current).object();
			auto authors = updated.value(u"hiddenAuthors"_q).toArray();
			if (const auto index = authors.toVariantList().indexOf(author); index >= 0) {
				authors.removeAt(index);
			} else {
				authors.push_back(author);
				updated.insert(u"enabled"_q, true);
			}
			updated.insert(u"hiddenAuthors"_q, authors);
			const auto bytes = QJsonDocument(updated).toJson(QJsonDocument::Compact);
			if (!ForAccount(session).Set(kRules, bytes)) {
				controller->showToast(tr::lng_nagram_filter_invalid(tr::now));
				return;
			}
			controller->showToast(tr::lng_nagram_filter_menu_saved(tr::now));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconBlock, &st::menuIconBlock);
	auto position = int(menu->actions().size());
	for (auto index = 0; index != position; ++index) {
		const auto tag = menu->actions()[index]->property("nagramMenuActionId");
		if (tag.isValid() && tag.toInt() == int(Menu::ActionId::Delete)) {
			position = index;
			break;
		}
	}
	Menu::Tag(menu->insertAction(position, std::move(widget)),
		Menu::ActionId::FilterAuthor);
}

} // namespace Nagram::Filters
