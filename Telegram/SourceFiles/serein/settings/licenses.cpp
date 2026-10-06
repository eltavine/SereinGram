#include "serein/settings/licenses.h"

#include "lang/lang_keys.h"
#include "ui/layers/generic_box.h"
#include "ui/layers/show.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"

#include <QtCore/QFile>

#include <array>

namespace Serein {
namespace {

struct Notice {
	const char *name = nullptr;
	const char *license = nullptr;
	const char *url = nullptr;
	const char *resource = nullptr;
};

constexpr auto kNotices = std::array{
	Notice{
		"OpenCC",
		"Apache-2.0",
		"https://github.com/BYVoid/OpenCC",
		":/serein/licenses/opencc.txt",
	},
	Notice{
		"marisa-trie",
		"BSD-2-Clause OR LGPL-2.1-or-later",
		"https://github.com/s-yata/marisa-trie",
		":/serein/licenses/marisa.txt",
	},
	Notice{
		"darts-clone",
		"BSD-2-Clause",
		"https://github.com/s-yata/darts-clone",
		":/serein/licenses/darts-clone.txt",
	},
	Notice{
		"RapidJSON",
		"MIT",
		"https://github.com/Tencent/rapidjson",
		":/serein/licenses/rapidjson.txt",
	},
	Notice{
		"quirc",
		"ISC",
		"https://github.com/dlbeer/quirc",
		":/serein/licenses/quirc.txt",
	},
	Notice{
		"WizardLoop CreationDate",
		"MIT",
		"https://github.com/WizardLoop/CreationDate",
		":/serein/licenses/regdate_points.txt",
	},
};

[[nodiscard]] QString ReadResource(const char *path) {
	auto file = QFile(QString::fromLatin1(path));
	return file.open(QIODevice::ReadOnly)
		? QString::fromUtf8(file.readAll())
		: QString();
}

void LicenseBox(not_null<Ui::GenericBox*> box, Notice notice) {
	box->setTitle(rpl::single(QString::fromLatin1(notice.name)));
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		ReadResource(notice.resource),
		st::boxLabel));
	label->setSelectable(true);
	box->setWidth(st::boxWideWidth);
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void LicensesBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_licenses());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_licenses_about(),
		st::boxLabel));
	for (const auto &notice : kNotices) {
		const auto text = QString::fromLatin1(notice.name)
			+ u" · "_q
			+ QString::fromLatin1(notice.license)
			+ u'\n'
			+ QString::fromLatin1(notice.url);
		const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			text,
			st::boxLabel));
		label->setSelectable(true);
		const auto view = box->addRow(object_ptr<Ui::LinkButton>(
			box,
			tr::lng_serein_licenses_view(tr::now)));
		view->setClickedCallback([=] {
			box->uiShow()->showBox(Box(LicenseBox, notice));
		});
	}
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

void ShowLicenses(gsl::not_null<Window::SessionController*> controller) {
	controller->show(Box(LicensesBox));
}

} // namespace Serein
