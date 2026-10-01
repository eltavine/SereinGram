#include "serein/hooks/tray.h"

#include "serein/privacy/options.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/interface.h"
#include "lang/lang_keys.h"
#include "platform/platform_tray.h"

namespace Serein::Hooks {

void FillTrayMenu(Platform::Tray &tray) {
	if (!ForDevice().Get(Serein::Interface::kMenuShortcuts)) {
		return;
	}
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
	tray.addAction(
		ForDevice().Value(
			Serein::Ghost::kGhostAllAccounts
		) | rpl::map([](bool enabled) {
			return enabled
				? tr::lng_serein_ghost_tray_disable(tr::now)
				: tr::lng_serein_ghost_tray_enable(tr::now);
		}),
		[] {
			auto &options = ForDevice();
			Expects(options.Set(
				Serein::Ghost::kGhostAllAccounts,
				!options.Get(Serein::Ghost::kGhostAllAccounts)));
		});
}

} // namespace Serein::Hooks
