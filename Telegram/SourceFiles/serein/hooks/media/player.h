#pragma once

namespace Ui { class RpWidget; }
namespace Window { class SessionController; }

namespace Serein::Media {

void AttachExpandedPlayer(
	gsl::not_null<Ui::RpWidget*> widget,
	gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Media
