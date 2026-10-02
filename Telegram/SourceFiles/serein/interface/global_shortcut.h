#pragma once

#include <gsl/pointers>

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Serein::Interface {

void StartGlobalShortcut();
void GlobalShortcutBox(gsl::not_null<Ui::GenericBox*> box);

} // namespace Serein::Interface
