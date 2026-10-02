#include "serein/filters/hide_message_menu.h"

#include "serein/filters/hidden_messages.h"
#include "serein/hooks/menu/actions.h"
#include "serein/schema/gen/settings/filters.h"
#include "data/data_peer_id.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Filters {
namespace {

[[nodiscard]] QStringList Tokens(not_null<HistoryItem*> item) {
	auto result = QStringList();
	const auto peer = SerializePeerId(item->history()->peer->id);
	for (const auto &id : item->history()->owner().itemOrItsGroup(item)) {
		result.push_back(Filters::HiddenMessageToken(peer, id.msg.bare));
	}
	return result;
}

} // namespace

void InsertHideMessageAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller
		|| !item->isRegular()
		|| item->isService()
		|| (&item->history()->session() != &controller->session())) {
		return;
	}
	const auto session = &controller->session();
	const auto tokens = Tokens(item);
	if (tokens.isEmpty()) {
		return;
	}
	const auto hidden = Filters::HiddenMessageSet(
		ForAccount(session).Get(Filters::kHiddenMessages)
	).contains(tokens.front());
	const auto action = Ui::Menu::CreateAction(
		menu,
		(hidden
			? tr::lng_serein_menu_show_message(tr::now)
			: tr::lng_serein_menu_hide_message(tr::now)),
		crl::guard(controller, [=] {
			auto &options = ForAccount(session);
			const auto updated = Filters::ToggleHiddenMessages(
				options.Get(Filters::kHiddenMessages),
				tokens);
			if (!options.Set(Filters::kHiddenMessages, updated)) {
				LOG(("Serein: could not store the hidden messages."));
			}
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(),
		menu->menu()->st(),
		action,
		&st::menuIconCaptionHide,
		&st::menuIconCaptionHide);
	Menu::Tag(
		menu->insertAction(Menu::DeleteActionIndex(menu), std::move(widget)),
		Menu::ActionId::HideMessage);
}

} // namespace Serein::Filters
