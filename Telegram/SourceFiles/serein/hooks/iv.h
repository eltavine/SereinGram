#pragma once

#include "base/basic_types.h"

#include <memory>

namespace Iv {
class Data;
struct RichPage;
} // namespace Iv

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace rpl {
class lifetime;
} // namespace rpl

namespace Serein::Hooks::Iv {

[[nodiscard]] std::shared_ptr<const ::Iv::RichPage> DisplayedPage(
	not_null<Main::Session*> session,
	not_null<::Iv::Data*> data);
void OnPageRefresh(
	not_null<Main::Session*> session,
	rpl::lifetime &lifetime,
	Fn<void(not_null<::Iv::Data*> data)> callback);
void FillMenu(not_null<Ui::PopupMenu*> menu, uint64 pageId);

} // namespace Serein::Hooks::Iv
