#pragma once

#include "nagram/menu/model.h"

class QAction;
namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Nagram::Menu {

void Tag(QAction *action, ActionId id);
void Apply(Ui::PopupMenu *menu);

} // namespace Nagram::Menu
