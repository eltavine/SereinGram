#include "serein/hooks/interface/main_menu.h"

#include "serein/interface/options.h"
#include "lang/lang_keys.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>

#include <algorithm>

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Serein::Interface {
namespace {

constexpr auto kMenuItemMime = "application/x-serein-main-menu-item";

class MenuOrderRow final : public Ui::SettingsButton {
public:
	MenuOrderRow(
			QWidget *parent,
			QString id,
			rpl::producer<QString> title,
			Fn<void(QString, QString)> moved)
	: Ui::SettingsButton(parent, std::move(title), st::settingsButtonNoIcon)
	, _id(std::move(id))
	, _moved(std::move(moved)) {
		setAcceptDrops(true);
	}

protected:
	void mousePressEvent(QMouseEvent *event) override {
		_dragStart = event->globalPosition().toPoint();
		Ui::SettingsButton::mousePressEvent(event);
	}

	void mouseMoveEvent(QMouseEvent *event) override {
		if ((event->buttons() & Qt::LeftButton)
			&& (event->globalPosition().toPoint() - _dragStart).manhattanLength()
				>= QApplication::startDragDistance()) {
			const auto data = new QMimeData();
			data->setData(kMenuItemMime, _id.toUtf8());
			const auto drag = new QDrag(this);
			drag->setMimeData(data);
			drag->exec(Qt::MoveAction);
			return;
		}
		Ui::SettingsButton::mouseMoveEvent(event);
	}

	void dragEnterEvent(QDragEnterEvent *event) override {
		if (event->mimeData()->hasFormat(kMenuItemMime)) {
			event->acceptProposedAction();
		}
	}

	void dropEvent(QDropEvent *event) override {
		const auto from = QString::fromUtf8(
			event->mimeData()->data(kMenuItemMime));
		if (!from.isEmpty() && from != _id) {
			_moved(from, _id);
		}
		event->acceptProposedAction();
	}

private:
	const QString _id;
	Fn<void(QString, QString)> _moved;
	QPoint _dragStart;

};

} // namespace

QString MainMenuActionTitle(const QString &id) {
	if (id == u"profile"_q) return tr::lng_serein_main_menu_action_profile(tr::now);
	if (id == u"bots"_q) return tr::lng_serein_main_menu_bots(tr::now);
	if (id == u"newGroup"_q) return tr::lng_serein_main_menu_action_new_group(tr::now);
	if (id == u"newChannel"_q) return tr::lng_serein_main_menu_action_new_channel(tr::now);
	if (id == u"contacts"_q) return tr::lng_serein_main_menu_action_contacts(tr::now);
	if (id == u"calls"_q) return tr::lng_serein_main_menu_action_calls(tr::now);
	if (id == u"savedMessages"_q) return tr::lng_serein_main_menu_action_saved_messages(tr::now);
	if (id == u"settings"_q) return tr::lng_serein_main_menu_action_settings(tr::now);
	return tr::lng_serein_main_menu_action_night_mode(tr::now);
}

std::optional<MainMenuConfig> MainMenu() {
	const auto bytes = ForDevice().Get(kMainMenuConfig);
	if (bytes.isEmpty()) {
		return MainMenuDefaults();
	}
	auto result = ParseMainMenuConfig(bytes);
	if (!result) {
		LOG(("Serein Error: Invalid mainMenu configuration; native menu retained."));
	}
	return result;
}

void SetMainMenu(const MainMenuConfig &value) {
	const auto bytes = (value == MainMenuDefaults())
		? QByteArray()
		: SerializeMainMenuConfig(value);
	Expects(ValidMainMenuBytes(bytes));
	Expects(ForDevice().Set(kMainMenuConfig, bytes));
}

QString MainMenuTitle() {
	const auto value = MainMenu();
	return value ? value->title : QString();
}

bool MainMenuSeasonal() {
	const auto value = MainMenu();
	return !value || value->seasonalDecorations;
}

bool MainMenuCustomOrder() {
	const auto value = MainMenu();
	return value && (!value->order.empty() || !value->hidden.empty());
}

not_null<Ui::VerticalLayout*> AddMainMenuGroup(
		not_null<Ui::VerticalLayout*> menu,
		const QString &id) {
	const auto config = MainMenu().value_or(MainMenuDefaults());
	const auto group = menu->add(object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
		menu,
		object_ptr<Ui::VerticalLayout>(menu)));
	group->setProperty("sereinMainMenuAction", id);
	group->toggle(
		ranges::find(config.hidden, id) == config.hidden.end(),
		anim::type::instant);
	const auto &order = config.order;
	const auto rank = [&](const QString &key) {
		return int(ranges::find(order, key) - order.begin());
	};
	for (auto i = 0; i + 1 < menu->count(); ++i) {
		const auto other = menu->widgetAt(i)->property(
			"sereinMainMenuAction").toString();
		if (!other.isEmpty() && rank(other) > rank(id)) {
			menu->reorderRows(menu->count() - 1, i);
			break;
		}
	}
	return group->entity();
}

void MainMenuBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_main_menu());

	const auto current = MainMenu();
	if (!current) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box, tr::lng_serein_main_menu_invalid(), st::boxLabel));
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_serein_main_menu_about(), st::boxLabel));
	const auto title = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		tr::lng_serein_main_menu_title(),
		current->title));
	title->setMaxLength(96);
	const auto seasonal = box->addRow(object_ptr<Ui::Checkbox>(
		box,
		tr::lng_serein_main_menu_seasonal(tr::now),
		current->seasonalDecorations));
	struct State {
		std::vector<QString> order;
		std::vector<QString> hidden;
		Fn<void()> refresh;
	};
	const auto state = box->lifetime().make_state<State>();
	state->order = current->order;
	state->hidden = current->hidden;
	const auto contains = [](const std::vector<QString> &list, const QString &id) {
		return ranges::find(list, id) != list.end();
	};
	for (const auto id : kMainMenuIds) {
		const auto text = QString::fromLatin1(id);
		if (!contains(state->order, text)) {
			state->order.push_back(text);
		}
	}
	const auto rows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
	state->refresh = [=] {
		rows->clear();
		for (auto index = 0; index != int(state->order.size()); ++index) {
			const auto id = state->order[index];
			const auto hidden = contains(state->hidden, id);
			const auto row = rows->add(object_ptr<MenuOrderRow>(
				rows,
				id,
				rpl::single(MainMenuActionTitle(id)),
				[=](QString from, QString to) {
					const auto fromIt = ranges::find(state->order, from);
					const auto toIt = ranges::find(state->order, to);
					if (fromIt == state->order.end()
						|| toIt == state->order.end()) {
						return;
					}
					const auto toIndex = int(toIt - state->order.begin());
					state->order.erase(fromIt);
					state->order.insert(state->order.begin() + toIndex, from);
					InvokeQueued(box, state->refresh);
				}));
			if (id != u"settings"_q) {
				row->toggleOn(rpl::single(!hidden));
				row->toggledChanges(
				) | rpl::on_next([=](bool shown) {
					if (shown) {
						const auto found = ranges::find(state->hidden, id);
						if (found != state->hidden.end()) {
							state->hidden.erase(found);
						}
					} else if (!contains(state->hidden, id)) {
						state->hidden.push_back(id);
					}
				}, row->lifetime());
			}
		}
	};
	box->addButton(tr::lng_settings_save(), [=] {
		if (MainMenu() != current) {
			box->showToast(tr::lng_serein_main_menu_changed(tr::now));
			return;
		}
		auto result = *current;
		result.title = title->getLastText().trimmed();
		result.seasonalDecorations = seasonal->checked();
		auto natural = std::vector<QString>();
		for (const auto id : kMainMenuIds) {
			natural.push_back(QString::fromLatin1(id));
		}
		result.order = (state->order == natural)
			? std::vector<QString>()
			: state->order;
		result.hidden = state->hidden;
		if (!ValidMainMenuBytes(SerializeMainMenuConfig(result))) {
			box->showToast(tr::lng_serein_main_menu_invalid(tr::now));
			return;
		}
		SetMainMenu(result);
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	state->refresh();
}

} // namespace Serein::Interface
