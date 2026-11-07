#pragma once

#include <gsl/pointers>

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Serein {

void AboutBox(gsl::not_null<Ui::GenericBox*> box);

} // namespace Serein
