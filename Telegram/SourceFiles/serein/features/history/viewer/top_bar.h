#pragma once

#include "base/object_ptr.h"
#include "base/unique_qptr.h"
#include "ui/rp_widget.h"

class PeerData;

namespace Profile {
class BackButton;
} // namespace Profile

namespace Ui {
class IconButton;
class PopupMenu;
} // namespace Ui

namespace Ui::Menu {
struct MenuCallback;
} // namespace Ui::Menu

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::HistoryFeature::Viewer {

class TopBar final : public Ui::RpWidget {
public:
	using FillMenu = Fn<void(const Ui::Menu::MenuCallback &add)>;

	TopBar(
		QWidget *parent,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer,
		rpl::producer<QString> subtitle,
		FillMenu fillMenu);
	~TopBar();

	// While animating the content is hidden and the bar acts as back.
	void setAnimatingMode(bool enabled);

protected:
	void paintEvent(QPaintEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	int resizeGetHeight(int newWidth) override;

private:
	void showMenu();

	const not_null<Window::SessionController*> _controller;
	const FillMenu _fillMenu;
	object_ptr<Profile::BackButton> _back;
	object_ptr<Ui::IconButton> _menuToggle;
	base::unique_qptr<Ui::PopupMenu> _menu;
	bool _animatingMode = false;

};

} // namespace Serein::HistoryFeature::Viewer
