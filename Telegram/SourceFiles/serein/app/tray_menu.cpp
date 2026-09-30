#include "serein/hooks/tray.h"

#include "serein/privacy/options.h"
#include "lang/lang_keys.h"
#include "platform/platform_tray.h"

namespace Serein::Hooks {

void FillTrayMenu(Platform::Tray &tray) {
	tray.addAction(
		ForDevice().Value(Privacy::kDemoMode) | rpl::map([](bool enabled) {
			return enabled
				? tr::lng_serein_demo_mode_disable(tr::now)
				: tr::lng_serein_demo_mode_enable(tr::now);
		}),
		[] {
			auto &options = ForDevice();
			Expects(options.Set(
				Privacy::kDemoMode,
				!options.Get(Privacy::kDemoMode)));
		});
}

} // namespace Serein::Hooks
