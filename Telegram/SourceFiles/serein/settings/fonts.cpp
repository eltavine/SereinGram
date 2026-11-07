#include "serein/settings/fonts.h"

#include "serein/interface/options.h"
#include "serein/settings/restart.h"
#include "serein/settings/rows.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "mainwindow.h"

#include <QtGui/QFontDatabase>
#include <QtWidgets/QFontDialog>

#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein {

using namespace Interface;
namespace {

void ChooseFont(not_null<Window::SessionController*> controller) {
	const auto current = ForDevice().Get(kMonospaceFont);
	const auto initial = current.isEmpty()
		? QFontDatabase::systemFont(QFontDatabase::FixedFont)
		: QFont(current);
	const auto dialog = new QFontDialog(initial, controller->widget().get());
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->setOption(QFontDialog::MonospacedFonts);
	const auto selected = crl::guard(controller, [=](const QFont &font) {
		if (font.family() != current) {
			Expects(ForDevice().Set(kMonospaceFont, font.family()));
			ShowRestartPrompt(controller);
		}
	});
	QObject::connect(dialog, &QFontDialog::fontSelected, dialog, [=](const QFont &font) {
		selected(font);
	});
	dialog->open();
}

} // namespace

void AddMonospaceFont(Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = u"serein/interface/monospace-font"_q,
		.title = tr::lng_serein_monospace_font(),
		.label = ForDevice().Value(kMonospaceFont) | rpl::map([](const QString &family) {
			return family.isEmpty() ? tr::lng_serein_font_default(tr::now) : family;
		}),
		.onClick = [=] {
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				box->setTitle(tr::lng_serein_monospace_font());
				const auto choose = box->addRow(object_ptr<Ui::SettingsButton>(
					box, tr::lng_serein_monospace_font(), st::settingsButtonNoIcon));
				choose->setClickedCallback([=] {
					box->closeBox();
					ChooseFont(controller);
				});
				box->addButton(tr::lng_serein_font_default(), [=] {
					if (!ForDevice().Get(kMonospaceFont).isEmpty()) {
						Expects(ForDevice().Set(kMonospaceFont, QString()));
						ShowRestartPrompt(controller);
					}
					box->closeBox();
				});
				box->addButton(tr::lng_close(), [=] { box->closeBox(); });
			}));
		},
		.keywords = { u"monospace"_q, u"font"_q, u"code"_q },
		.visual = {
			.icon = &st::menuIconFont,
			.about = tr::lng_serein_monospace_font_about,
		},
	});
}

} // namespace Serein
