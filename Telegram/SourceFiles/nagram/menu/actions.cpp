#include "nagram/menu/actions.h"
#include "nagram/menu/repeat.h"

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
		Window::SessionController *controller) {
	Expects(menu != nullptr);
	if (item && controller) {
		InsertRepeatActions(menu, item, controller);
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
