#include "serein/privacy/qr_scan.h"

#include "serein/privacy/login_token.h"
#include "serein/privacy/qr_decode.h"
#include "apiwrap.h"
#include "base/call_delayed.h"
#include "core/application.h"
#include "core/click_handler_types.h"
#include "core/file_utilities.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "mtproto/mtproto_response.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <QtCore/QUrl>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QScreen>

namespace Serein::Privacy {
namespace {

constexpr auto kScreenDelay = crl::time(300);

[[nodiscard]] QStringList Decode(const QImage &image) {
	if (image.isNull()) {
		return QStringList();
	}
	const auto gray = image.convertToFormat(QImage::Format_Grayscale8);
	return DecodeQrCodes({
		.pixels = gray.constBits(),
		.width = gray.width(),
		.height = gray.height(),
		.stride = int(gray.bytesPerLine()),
	});
}

[[nodiscard]] QStringList DecodeScreens() {
	auto result = QStringList();
	for (const auto screen : QGuiApplication::screens()) {
		result += Decode(screen->grabWindow(0).toImage());
	}
	result.removeDuplicates();
	return result;
}

void AcceptLogin(
		not_null<Window::SessionController*> controller,
		const QByteArray &token) {
	controller->session().api().request(MTPauth_AcceptLoginToken(
		MTP_bytes(token)
	)).done(crl::guard(controller, [=](const MTPAuthorization &result) {
		const auto &data = result.data();
		controller->showToast(tr::lng_serein_qr_login_done(
			tr::now,
			lt_device,
			qs(data.vapp_name()) + u", "_q + qs(data.vdevice_model())));
	})).fail(crl::guard(controller, [=](const MTP::Error &error) {
		if (error.type().startsWith(u"AUTH_TOKEN_"_q)) {
			controller->showToast(tr::lng_serein_qr_login_expired(tr::now));
		} else {
			MTP::ShowErrorFallback(controller->uiShow(), error);
		}
	})).send();
}

void ConfirmLogin(
		not_null<Window::SessionController*> controller,
		const QByteArray &token) {
	controller->show(Ui::MakeConfirmBox({
		.text = tr::lng_serein_qr_login_sure(),
		.confirmed = [=](Fn<void()> &&close) {
			close();
			AcceptLogin(controller, token);
		},
		.confirmText = tr::lng_serein_qr_login_confirm(),
		.confirmStyle = &st::attentionBoxButton,
	}));
}

void ShowCodes(
		not_null<Window::SessionController*> controller,
		const QStringList &codes) {
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_qr_result());
		for (const auto &code : codes) {
			const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
				box,
				code,
				st::boxLabel));
			label->setSelectable(true);
			label->setBreakEverywhere(true);
			const auto url = QUrl(code, QUrl::StrictMode);
			const auto scheme = url.scheme();
			if (!url.isValid()
				|| (scheme != u"tg"_q
					&& scheme != u"https"_q
					&& scheme != u"http"_q)) {
				continue;
			}
			const auto open = box->addRow(object_ptr<Ui::LinkButton>(
				box,
				tr::lng_open_link(tr::now)));
			open->setClickedCallback([=] {
				box->closeBox();
				HiddenUrlClickHandler::Open(
					code,
					QVariant::fromValue(ClickHandlerContext{
						.sessionWindow = base::make_weak(controller),
					}));
			});
		}
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

void Handle(
		not_null<Window::SessionController*> controller,
		const QStringList &codes) {
	if (codes.isEmpty()) {
		controller->showToast(tr::lng_serein_qr_none(tr::now));
		return;
	}
	for (const auto &code : codes) {
		if (const auto token = ParseLoginToken(code)) {
			ConfirmLogin(controller, *token);
			return;
		}
	}
	ShowCodes(controller, codes);
}

void ChooseImage(not_null<Window::SessionController*> controller) {
	FileDialog::GetOpenPath(
		Core::App().getFileDialogParent(),
		tr::lng_serein_qr_from_file(tr::now),
		u"Images (*.png *.jpg *.jpeg *.bmp *.webp *.gif)"_q,
		crl::guard(controller, [=](FileDialog::OpenResult &&result) {
			const auto image = !result.remoteContent.isEmpty()
				? QImage::fromData(result.remoteContent)
				: result.paths.isEmpty()
				? QImage()
				: QImage(result.paths.front());
			Handle(controller, Decode(image));
		}));
}

} // namespace

void ShowQrScanner(gsl::not_null<Window::SessionController*> controller) {
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_qr_scan());
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_qr_scan_about(),
			st::boxLabel));
		const auto add = [&](rpl::producer<QString> text, Fn<void()> handler) {
			const auto button = box->addRow(object_ptr<Ui::SettingsButton>(
				box,
				std::move(text),
				st::settingsButtonNoIcon));
			button->setClickedCallback(std::move(handler));
		};
		add(tr::lng_serein_qr_from_screen(), [=] {
			box->closeBox();
			base::call_delayed(kScreenDelay, controller, [=] {
				Handle(controller, DecodeScreens());
			});
		});
		add(tr::lng_serein_qr_from_clipboard(), [=] {
			box->closeBox();
			Handle(controller, Decode(QGuiApplication::clipboard()->image()));
		});
		add(tr::lng_serein_qr_from_file(), [=] {
			box->closeBox();
			ChooseImage(controller);
		});
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

} // namespace Serein::Privacy
