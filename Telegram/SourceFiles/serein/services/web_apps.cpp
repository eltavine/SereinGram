#include "serein/hooks/services/web_apps.h"

#include "serein/hooks/gen/services.h"

namespace Serein::Hooks {

QString WebAppPlatform() {
	return ServiceSettings::AndroidWebApps() ? u"android"_q : u"tdesktop"_q;
}

} // namespace Serein::Hooks
