#pragma once

#include "base/basic_types.h"

class History;
namespace Main { class Session; }
namespace Ui::Menu { struct MenuCallback; }

namespace Serein::Chats {

[[nodiscard]] bool AllowedInFolder(
	gsl::not_null<::History*> history,
	int32 folderId);
void AddManagedOnlyAction(
	const Ui::Menu::MenuCallback &addAction,
	gsl::not_null<Main::Session*> session,
	int32 folderId);

} // namespace Serein::Chats
