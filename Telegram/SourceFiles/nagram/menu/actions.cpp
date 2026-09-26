#include "nagram/menu/actions.h"
#include "nagram/menu/repeat.h"
#include "nagram/menu/batch.h"
#include "nagram/menu/media.h"

#include "ui/widgets/popup_menu.h"

#include <QtGui/QAction>
#include <QtGui/QGuiApplication>

namespace Nagram::Menu {
namespace {

constexpr auto kActionIdProperty = "nagramMenuActionId";

} // namespace

void Tag(QAction *action, ActionId id) {
	Expects(action != nullptr);
	action->setProperty(kActionIdProperty, static_cast<int>(id));
}

void Apply(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller,
		MessageIdsList selected,
		Fn<void(HistoryItem*)> selectAuthor) {
	Expects(menu != nullptr);
	if (controller) {
		if (item) {
			InsertRepeatActions(menu, item, controller);
		}
		if (item || !selected.empty()) {
			InsertBatchActions(menu, item, controller,
				std::move(selected), std::move(selectAuthor));
		}
		if (item) {
			InsertMediaInfoAction(menu, item, controller);
		}
	}
	const auto config = ForDevice().Get(kMenuConfig);
	const auto optionHeld = (QGuiApplication::keyboardModifiers()
		& Qt::AltModifier) != 0;
	auto removedUpstreamAction = false;
	for (auto index = int(menu->actions().size()); index != 0;) {
		--index;
		const auto value = menu->actions()[index]->property(kActionIdProperty);
		if (value.isValid() && !Visible(
				ReadVisibility(config, ActionId(value.toInt())), optionHeld)) {
			removedUpstreamAction |= value.toInt() < int(ActionId::Repeat);
			menu->removeAction(index);
		}
	}
	if (!removedUpstreamAction) {
		return;
	}
	auto previousSeparator = true;
	for (auto index = 0; index != int(menu->actions().size());) {
		const auto separator = menu->actions()[index]->isSeparator();
		if (separator && previousSeparator) {
			menu->removeAction(index);
		} else {
			previousSeparator = separator;
			++index;
		}
	}
	if (!menu->actions().empty() && menu->actions().back()->isSeparator()) {
		menu->removeAction(int(menu->actions().size()) - 1);
	}
}

} // namespace Nagram::Menu
