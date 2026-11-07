#pragma once

#include <gsl/pointers>

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein {

void LicensesBox(gsl::not_null<Ui::GenericBox*> box);
void ShowLicenses(gsl::not_null<Window::SessionController*> controller);

} // namespace Serein
