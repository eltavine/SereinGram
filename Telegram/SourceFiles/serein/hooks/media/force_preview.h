#pragma once

#include <gsl/pointers>

namespace Ui {
class RpWidget;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Hooks::Media {

void InstallForcePreview(
	gsl::not_null<Ui::RpWidget*> widget,
	gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Hooks::Media
