#pragma once

#include "serein/ports/native_settings.h"

#include <gsl/pointers>

namespace Main { class Session; }

namespace Serein::Adapters {

[[nodiscard]] QJsonObject ExportNativeSettings(gsl::not_null<Main::Session*> session);
[[nodiscard]] NativeSettingsPlan PlanNativeSettings(
	gsl::not_null<Main::Session*> session,
	const QJsonObject &values);

} // namespace Serein::Adapters
