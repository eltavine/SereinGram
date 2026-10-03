#pragma once

#include "serein/core/options.h"
#include "serein/menu/model.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/schema/gen/settings/compose.h"
#include "serein/schema/gen/settings/filters.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/schema/gen/settings/links.h"
#include "serein/schema/gen/settings/media.h"
#include "serein/schema/gen/settings/menu.h"
#include "serein/schema/gen/settings/messages.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/schema/gen/settings/snapshot.h"

namespace Serein::Tests {

[[nodiscard]] inline Registry FullRegistry() {
	auto registry = Registry();
	Chats::RegisterOptions(registry);
	Compose::RegisterOptions(registry);
	Filters::RegisterOptions(registry);
	Ghost::RegisterOptions(registry);
	HistorySettings::RegisterOptions(registry);
	Interface::RegisterOptions(registry);
	Links::RegisterOptions(registry);
	Media::RegisterOptions(registry);
	Menu::RegisterOptions(registry);
	Messages::RegisterOptions(registry);
	Privacy::RegisterOptions(registry);
	ServiceSettings::RegisterOptions(registry);
	Snapshot::RegisterOptions(registry);
	return registry;
}

} // namespace Serein::Tests
