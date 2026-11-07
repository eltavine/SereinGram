#include "serein/hooks/chats/list_tools.h"

#include "serein/chats/options.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include <array>
#include <QtCore/QPointer>

#include "styles/style_dialogs.h"
#include "styles/style_menu_icons.h"
#include "styles/style_widgets.h"

namespace Serein::Chats {

void AttachChatListTools(
		gsl::not_null<Ui::InputField*> field,
		gsl::not_null<Window::SessionController*> controller) {
	struct Layout {
		QRect original;
		bool updating = false;
	};
	const auto layout = field->lifetime().make_state<Layout>();
	const auto admin = Ui::CreateChild<Ui::IconButton>(field->parentWidget(), st::dialogsMenuToggle);
	const auto sort = Ui::CreateChild<Ui::IconButton>(field->parentWidget(), st::dialogsMenuToggle);
	admin->setIconOverride(&st::menuIconAdmin);
	sort->setIconOverride(&st::menuIconOrderDate);
	admin->setAccessibleName(tr::lng_serein_admin_chats(tr::now));
	sort->setAccessibleName(tr::lng_serein_sort_date(tr::now));
	const auto apply = [=] {
		if (layout->updating) return;
		layout->updating = true;
		const auto total = admin->width() + sort->width();
		const auto visible = ForDevice().Get(kChatListTools) && field->isVisible()
			&& layout->original.width() > total + 120;
		field->setGeometry(layout->original.adjusted(0, 0, visible ? -total : 0, 0));
		admin->move(layout->original.right() - total + 1, layout->original.center().y() - admin->height() / 2);
		sort->move(layout->original.right() - sort->width() + 1, admin->y());
		admin->setVisible(visible);
		sort->setVisible(visible);
		layout->updating = false;
	};
	field->geometryValue() | rpl::on_next([=](QRect geometry) {
		if (layout->updating) return;
		layout->original = geometry;
		apply();
	}, field->lifetime());
	field->shownValue() | rpl::on_next(apply, field->lifetime());
	ForDevice().Value(kChatListTools) | rpl::on_next(apply, field->lifetime());
	admin->setClickedCallback([=] {
		auto &options = ForAccount(&controller->session());
		Expects(options.Set(kOnlyManagedChats, !options.Get(kOnlyManagedChats)));
	});
	ForAccount(&controller->session()).Value(kOnlyManagedChats) | rpl::on_next([=](bool selected) {
		admin->setIconOverride(selected ? &st::menuIconTagFilter : &st::menuIconAdmin);
	}, field->lifetime());
	sort->setClickedCallback([=] {
		const auto menu = Ui::CreateChild<Ui::PopupMenu>(sort, st::popupMenuWithIcons);
		const auto titles = std::array{
			tr::lng_serein_sort_date_default(tr::now),
			tr::lng_serein_sort_date_newest(tr::now),
			tr::lng_serein_sort_date_oldest(tr::now),
		};
		for (auto i = 0; i != int(titles.size()); ++i) {
			menu->addAction(titles[i], [=] { Expects(ForDevice().Set(kChatDateOrder, i)); });
		}
		menu->popup(sort->mapToGlobal(QPoint(0, sort->height())));
	});
	const auto weakAdmin = QPointer<Ui::IconButton>(admin);
	const auto weakSort = QPointer<Ui::IconButton>(sort);
	field->lifetime().add([=] { delete weakAdmin.data(); delete weakSort.data(); });
}

} // namespace Serein::Chats
