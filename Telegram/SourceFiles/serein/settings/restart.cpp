#include "serein/settings/restart.h"

#include "core/application.h"
#include "lang/lang_keys.h"
#include "ui/boxes/confirm_box.h"
#include "window/window_session_controller.h"

namespace Serein {

void ShowRestartPrompt(gsl::not_null<Window::SessionController*> controller) {
	controller->show(Ui::MakeConfirmBox({
		.text = tr::lng_settings_need_restart(),
		.confirmed = [] { Core::Restart(); },
		.confirmText = tr::lng_settings_restart_now(),
		.cancelText = tr::lng_settings_restart_later(),
	}));
}

} // namespace Serein
