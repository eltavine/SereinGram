#pragma once

#include <gsl/pointers>

namespace Ui {
class RpWidget;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace HistoryView {
class ListDelegate;
} // namespace HistoryView

namespace Serein::Hooks::Media {

void InstallForcePreview(
	gsl::not_null<Ui::RpWidget*> widget,
	gsl::not_null<Window::SessionController*> controller);
void InstallForcePreview(
	gsl::not_null<Ui::RpWidget*> widget,
	gsl::not_null<HistoryView::ListDelegate*> delegate);

} // namespace Serein::Hooks::Media
