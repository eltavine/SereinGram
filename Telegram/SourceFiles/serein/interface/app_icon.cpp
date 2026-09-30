#include "serein/interface/app_icon.h"

#include "base/custom_app_icon.h"
#include "core/application.h"
#include "core/file_utilities.h"
#include "lang/lang_keys.h"
#include "settings.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/main_window.h"
#include "styles/style_layers.h"

#include <QtCore/QFile>
#include <QtGui/QImage>

#include <rpl/variable.h>

namespace Serein::Interface {
namespace {

constexpr auto kMinimumSide = 64;
constexpr auto kStoredSide = 512;

[[nodiscard]] QString IconPath() {
	return cWorkingDir() + u"tdata/serein_app_icon.png"_q;
}

rpl::variable<bool> &Custom() {
	static auto value = rpl::variable<bool>(false);
	return value;
}

void Apply(const QImage &image) {
	Window::OverrideApplicationIcon(image);
	Core::App().refreshApplicationIcon();
	Custom() = !image.isNull();
}

void Choose(gsl::not_null<Ui::GenericBox*> box) {
	const auto show = box->uiShow();
	FileDialog::GetOpenPath(
		box.get(),
		tr::lng_serein_app_icon_choose(tr::now),
		u"Image files (*.png *.jpg *.jpeg *.webp);;"_q
			+ FileDialog::AllFilesFilter(),
		crl::guard(box, [=](FileDialog::OpenResult &&result) {
			auto image = result.paths.isEmpty()
				? QImage()
				: QImage(result.paths.front());
			if (image.width() < kMinimumSide || image.height() < kMinimumSide) {
				show->showToast(tr::lng_serein_app_icon_invalid(tr::now));
				return;
			} else if (image.width() > kStoredSide
				|| image.height() > kStoredSide) {
				image = image.scaled(
					kStoredSide,
					kStoredSide,
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation);
			}
			if (!image.save(IconPath(), "PNG")) {
				show->showToast(tr::lng_serein_app_icon_failed(tr::now));
				return;
			}
			base::SetCustomAppIcon(image);
			Apply(image);
			box->closeBox();
		}));
}

} // namespace

void StartAppIcon() {
	if (QFile::exists(IconPath())) {
		const auto image = QImage(IconPath());
		if (!image.isNull()) {
			Apply(image);
		}
	}
}

rpl::producer<bool> CustomAppIconValue() {
	return Custom().value();
}

void AppIconBox(gsl::not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_app_icon());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_app_icon_about(),
		st::boxLabel));
	box->addButton(tr::lng_serein_app_icon_choose(), [=] {
		Choose(box);
	});
	if (Custom().current()) {
		box->addLeftButton(tr::lng_serein_app_icon_reset(), [=] {
			QFile::remove(IconPath());
			base::ClearCustomAppIcon();
			Apply(QImage());
			box->closeBox();
		});
	}
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace Serein::Interface
