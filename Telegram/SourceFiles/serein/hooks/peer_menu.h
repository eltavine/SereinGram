#pragma once

#include <gsl/pointers>

class PeerData;

namespace Data {
class ForumTopic;
} // namespace Data

namespace Ui::Menu {
struct MenuCallback;
} // namespace Ui::Menu

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Hooks {

void FillHistoryMenu(
	const Ui::Menu::MenuCallback &addAction,
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer,
	Data::ForumTopic *topic);
void FillProfileMenu(
	const Ui::Menu::MenuCallback &addAction,
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer,
	Data::ForumTopic *topic);

} // namespace Serein::Hooks
