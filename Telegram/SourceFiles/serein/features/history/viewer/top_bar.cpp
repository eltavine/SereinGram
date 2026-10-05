#include "serein/features/history/viewer/top_bar.h"

#include "info/profile/info_profile_values.h"
#include "profile/profile_back_button.h"
#include "ui/controls/userpic_button.h"
#include "ui/effects/panel_animation.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "ui/widgets/menu/menu_add_action_callback_factory.h"
#include "ui/widgets/popup_menu.h"
#include "ui/widgets/shadow.h"
#include "window/window_session_controller.h"
#include "styles/style_chat.h"
#include "styles/style_chat_helpers.h"
#include "styles/style_info.h"
#include "styles/style_menu_icons.h"
#include "styles/style_window.h"

namespace Serein::HistoryFeature::Viewer {

TopBar::TopBar(
	QWidget *parent,
	not_null<Window::SessionController*> controller,
	not_null<PeerData*> peer,
	rpl::producer<QString> subtitle,
	FillMenu fillMenu)
: RpWidget(parent)
, _controller(controller)
, _fillMenu(std::move(fillMenu))
, _back(this)
, _menuToggle(this, st::topBarMenuToggle) {
	_back->moveToLeft(0, 0);
	_back->setClickedCallback([=] {
		_controller->showBackFromStack();
	});
	_back->setWidget(Ui::CreateChild<Ui::UserpicButton>(
		_back.get(),
		peer,
		st::topBarInfoButton));
	Info::Profile::NameValue(peer) | rpl::on_next([=](QString name) {
		_back->setText(name);
	}, _back->lifetime());
	std::move(subtitle) | rpl::on_next([=](QString text) {
		_back->setSubtext(text);
	}, _back->lifetime());
	_menuToggle->setClickedCallback([=] {
		showMenu();
	});
	_menuToggle->setVisible(_fillMenu != nullptr);
}

TopBar::~TopBar() = default;

void TopBar::setAnimatingMode(bool enabled) {
	if (_animatingMode == enabled) {
		return;
	}
	_animatingMode = enabled;
	setCursor(enabled ? style::cur_pointer : style::cur_default);
	if (enabled) {
		setAttribute(Qt::WA_OpaquePaintEvent, false);
		hideChildren();
	} else {
		setAttribute(Qt::WA_OpaquePaintEvent);
		showChildren();
		_menuToggle->setVisible(_fillMenu != nullptr);
	}
	show();
}

void TopBar::paintEvent(QPaintEvent *e) {
	if (!_animatingMode) {
		auto p = QPainter(this);
		p.fillRect(e->rect(), st::topBarBg);
	}
}

void TopBar::mousePressEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		_controller->showBackFromStack();
	} else {
		RpWidget::mousePressEvent(e);
	}
}

int TopBar::resizeGetHeight(int newWidth) {
	const auto right = st::historySendRight + st::lineWidth;
	const auto menuLeft = newWidth - _menuToggle->width() - right;
	_menuToggle->moveToLeft(menuLeft, 0);
	_back->resizeToWidth(_menuToggle->isHidden() ? newWidth : menuLeft);
	_back->moveToLeft(0, 0);
	return _back->height();
}

void TopBar::showMenu() {
	if (_menu || !_fillMenu) {
		return;
	}
	_menu = base::make_unique_q<Ui::PopupMenu>(
		this,
		st::popupMenuExpandedSeparator);
	_fillMenu(Ui::Menu::CreateAddActionCallback(_menu));
	if (_menu->empty()) {
		_menu = nullptr;
		return;
	}
	_menu->setDestroyedCallback(crl::guard(this, [=] {
		_menu = nullptr;
	}));
	_menu->setForcedOrigin(Ui::PanelAnimation::Origin::TopRight);
	_menu->popup(Ui::PopupMenu::ConstrainToParentScreen(
		_menu,
		mapToGlobal(QPoint(
			width() + st::topBarMenuPosition.x(),
			st::topBarMenuPosition.y()))));
}

} // namespace Serein::HistoryFeature::Viewer
