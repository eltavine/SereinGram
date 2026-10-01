#pragma once

#include "base/basic_types.h"

#include <vector>

class History;
namespace Data { class ChatFilter; }
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
[[nodiscard]] bool AllChatsHidden(gsl::not_null<Main::Session*> session);
[[nodiscard]] std::vector<Data::ChatFilter> ShownFilters(
	gsl::not_null<Main::Session*> session);
[[nodiscard]] int ShownFiltersLimit(gsl::not_null<Main::Session*> session);
void SaveShownOrder(
	gsl::not_null<Main::Session*> session,
	const std::vector<int32> &order);

} // namespace Serein::Chats
