#include "serein/hooks/media/force_preview.h"

#include "base/event_filter.h"
#include "base/platform/base_platform_haptic.h"
#include "data/data_document.h"
#include "data/data_file_origin.h"
#include "data/data_media_types.h"
#include "data/data_photo.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "history/view/history_view_list_widget.h"
#include "main/main_session.h"
#include "mainwindow.h"
#include "serein/hooks/gen/media.h"
#include "ui/click_handler.h"
#include "ui/rp_widget.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include <QtGui/QGuiApplication>

namespace Serein::Hooks::Media {

void InstallForcePreview(
		not_null<Ui::RpWidget*> widget,
		not_null<Window::SessionController*> controller) {
	const auto shown = widget->lifetime().make_state<bool>(false);
	controller->window().widget()->globalForceClicks(
	) | rpl::filter([](QPoint) {
		return ForceClickPreview();
	}) | rpl::on_next([=](QPoint global) {
		if (!widget->isVisible()
			|| !(QGuiApplication::mouseButtons() & Qt::LeftButton)
			|| !widget->visibleRegion().contains(widget->mapFromGlobal(global))) {
			return;
		}
		const auto view = HistoryView::Element::Hovered();
		const auto item = view ? view->data().get() : nullptr;
		const auto media = item ? item->media() : nullptr;
		if (!media || (&item->history()->session() != &controller->session())) {
			return;
		}
		const auto window = controller->widget();
		const auto document = media->document();
		const auto photo = document ? nullptr : media->photo();
		const auto previewed = document
			? window->showMediaPreview(item->fullId(), document)
			: photo
			? window->showMediaPreview(item->fullId(), photo)
			: false;
		if (previewed) {
			*shown = true;
			ClickHandler::unpressed();
			base::Platform::Haptic();
		}
	}, widget->lifetime());
	base::install_event_filter(widget, [=](not_null<QEvent*> e) {
		if (*shown && e->type() == QEvent::MouseButtonRelease) {
			*shown = false;
			controller->widget()->hideMediaPreview();
		}
		return base::EventFilterResult::Continue;
	}, widget->lifetime());
}

void InstallForcePreview(
		not_null<Ui::RpWidget*> widget,
		not_null<HistoryView::ListDelegate*> delegate) {
	if (delegate->listContext() != HistoryView::Context::ChatPreview) {
		InstallForcePreview(widget, delegate->listWindow());
	}
}

} // namespace Serein::Hooks::Media
