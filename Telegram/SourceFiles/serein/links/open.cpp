#include "serein/hooks/links/open.h"

#include "serein/links/model.h"
#include "core/application.h"
#include "core/click_handler_types.h"
#include "core/file_utilities.h"
#include "lang/lang_keys.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"

namespace Serein::Links {

bool HandleExternalLink(const QString &url, const QVariant &context) {
	const auto parsed = QUrl(url, QUrl::StrictMode);
	if (parsed.scheme() != u"http"_q && parsed.scheme() != u"https"_q) {
		return false;
	}
	const auto raw = ForDevice().Get(kRules);
	const auto result = Rewrite(raw, url);
	const auto rules = ReadRules(raw);
	if (result.error.isEmpty() && !result.changed
		&& !(rules && rules->confirmAll)) {
		return false;
	}
	const auto click = context.value<ClickHandlerContext>();
	const auto active = Core::App().activeWindow();
	const auto window = active ? active : Core::App().activePrimaryWindow();
	if (!click.show && !window) {
		LOG(("Serein link: confirmation requires an application window; reopen the link from Serein."));
		return true;
	}
	const auto show = [&](object_ptr<Ui::BoxContent> box) {
		if (click.show) {
			click.show->showBox(std::move(box));
		} else if (window) {
			window->show(std::move(box));
			window->activate();
		}
	};
	if (!result.error.isEmpty()) {
		LOG(("Serein link: %1.").arg(result.error));
		show(Ui::MakeInformBox(result.error));
		return true;
	}
	const auto destination = result.url.toString(QUrl::FullyEncoded);
	show(Box([=](not_null<Ui::GenericBox*> box) {
		Ui::ConfirmBox(box, {
			.text = result.changed
				? tr::lng_serein_link_changed(tr::now)
				: tr::lng_open_this_link(tr::now),
			.confirmed = [=](Fn<void()> close) {
				close();
				File::OpenUrl(destination);
			},
			.confirmText = tr::lng_open_link(),
		});
		const auto label = box->addRow(object_ptr<Ui::FlatLabel>(box,
			rpl::single(result.changed
				? url + u"\n↓\n"_q + destination : destination), st::boxLabel));
		label->setSelectable(true);
	}));
	return true;
}

} // namespace Serein::Links
