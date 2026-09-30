#pragma once

#include "base/basic_types.h"

namespace Main { class Session; }

namespace Serein::Chats {

[[nodiscard]] int32 StartupFolder(
	gsl::not_null<Main::Session*> session,
	int32 defaultId);
void VerifyStartupFolder(gsl::not_null<Main::Session*> session);
void RememberFolder(gsl::not_null<Main::Session*> session, int32 id);

} // namespace Serein::Chats
