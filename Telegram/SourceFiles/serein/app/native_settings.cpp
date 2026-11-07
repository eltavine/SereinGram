#include "serein/hooks/settings/native.h"

#include "serein/adapters/tdesktop/native_settings.h"

namespace Serein::Hooks {

QJsonObject ExportNativeSettings(gsl::not_null<Main::Session*> session) {
	return Adapters::ExportNativeSettings(session);
}

NativeSettingsPlan PlanNativeSettings(
		gsl::not_null<Main::Session*> session,
		const QJsonObject &values) {
	return Adapters::PlanNativeSettings(session, values);
}

} // namespace Serein::Hooks
