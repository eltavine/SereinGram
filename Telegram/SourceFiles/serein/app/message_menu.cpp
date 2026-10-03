#include "serein/app/message_menu.h"

#include "serein/features/ghost/read_until_here.h"
#include "serein/features/history/menu.h"
#include "serein/filters/hide_message_menu.h"
#include "serein/filters/menu.h"
#include "serein/menu/batch.h"
#include "serein/menu/buttons.h"
#include "serein/menu/contributors.h"
#include "serein/menu/details.h"
#include "serein/menu/media.h"
#include "serein/menu/rating.h"
#include "serein/menu/reminder.h"
#include "serein/menu/repeat.h"
#include "serein/messages/markdown_menu.h"
#include "serein/messages/reading_menu.h"
#include "serein/snapshot/snapshot.h"

namespace Serein::App {
namespace {

using ItemAction = void (*)(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

void AddItemAction(ItemAction action) {
	Menu::AddContributor([=](const Menu::Context &context) {
		if (context.item) {
			action(context.menu, context.item, context.controller);
		}
	});
}

void AddSelectionActions() {
	Menu::AddContributor([](const Menu::Context &context) {
		if (!context.item && context.selected.empty()) {
			return;
		}
		Menu::InsertBatchActions(
			context.menu,
			context.item,
			context.controller,
			context.selected,
			context.selection);
		Snapshot::InsertAction(
			context.menu,
			context.controller,
			context.item,
			context.selected);
	});
}

} // namespace

void RegisterMessageMenu() {
	AddItemAction(Menu::InsertQuickRatingActions);
	AddItemAction(Menu::InsertRepeatActions);
	AddItemAction(Menu::InsertReminderAction);
	AddSelectionActions();
	AddItemAction(Menu::InsertMediaInfoAction);
	AddItemAction(Messages::InsertReadingAction);
	AddItemAction(Filters::InsertAuthorAction);
	AddItemAction(HistoryFeature::InsertEditHistoryAction);
	AddItemAction(HistoryFeature::InsertDeletedMessagesAction);
	AddItemAction(Ghost::InsertReadUntilHereAction);
	AddItemAction(HistoryFeature::InsertHistoryExclusionAction);
	AddItemAction(Menu::InsertButtonDataAction);
	AddItemAction(Menu::InsertDetailsAction);
	AddItemAction(Messages::InsertCopyMarkdownAction);
	AddItemAction(Filters::InsertHideMessageAction);
}

} // namespace Serein::App
