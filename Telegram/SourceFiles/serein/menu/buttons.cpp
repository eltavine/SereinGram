#include "serein/menu/buttons.h"

#include "serein/menu/actions.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "lang/lang_keys.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>

#include <algorithm>

namespace Serein::Menu {
namespace {

struct ButtonData {
	QString text;
	QString data;
};

[[nodiscard]] QString Describe(const QByteArray &data) {
	const auto text = QString::fromUtf8(data);
	const auto printable = (text.toUtf8() == data)
		&& std::none_of(text.begin(), text.end(), [](QChar ch) {
			return ch.category() == QChar::Other_Control;
		});
	return printable
		? text
		: (u"base64:"_q + QString::fromLatin1(data.toBase64()));
}

[[nodiscard]] std::vector<ButtonData> Collect(not_null<HistoryItem*> item) {
	auto result = std::vector<ButtonData>();
	if (const auto markup = item->Get<HistoryMessageReplyMarkup>()) {
		for (const auto &row : markup->data.rows) {
			for (const auto &button : row) {
				if (!button.data.isEmpty()) {
					result.push_back({ button.text, Describe(button.data) });
				}
			}
		}
	}
	return result;
}

void ButtonDataBox(
		not_null<Ui::GenericBox*> box,
		std::vector<ButtonData> entries) {
	box->setTitle(tr::lng_serein_menu_button_data());
	for (const auto &entry : entries) {
		const auto data = entry.data;
		const auto row = box->addRow(object_ptr<Ui::SettingsButton>(
			box,
			rpl::single(entry.text + u": "_q + data),
			st::settingsButtonNoIcon));
		row->setClickedCallback([=] {
			QGuiApplication::clipboard()->setText(data);
			box->showToast(tr::lng_text_copied(tr::now));
		});
	}
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void InsertButtonDataAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller) {
		return;
	}
	auto entries = Collect(item);
	if (entries.empty()) {
		return;
	}
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_menu_button_data(tr::now),
		crl::guard(controller, [=] {
			controller->show(Box(ButtonDataBox, entries));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconCopy, &st::menuIconCopy);
	Tag(menu->insertAction(DeleteActionIndex(menu), std::move(widget)),
		ActionId::ButtonData);
}

} // namespace Serein::Menu
