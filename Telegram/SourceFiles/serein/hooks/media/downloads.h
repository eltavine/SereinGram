#pragma once

#include "base/basic_types.h"

#include <vector>

struct FullMsgId;
struct GlobalMsgId;
#include <rpl/lifetime.h>

namespace Main { class Session; }
namespace Ui::Menu { struct MenuCallback; }
class HistoryItem;

namespace Serein::Media {

[[nodiscard]] bool DownloadVisible(
	gsl::not_null<Main::Session*> session,
	gsl::not_null<const HistoryItem*> item);
void WatchDownloadFilters(
	gsl::not_null<Main::Session*> session,
	Fn<void()> refresh,
	rpl::lifetime &lifetime);
void FillDownloadFilters(
	gsl::not_null<Main::Session*> session,
	const Ui::Menu::MenuCallback &addAction);
[[nodiscard]] std::vector<GlobalMsgId> VisibleDownloadIds(
	gsl::not_null<Main::Session*> session);

} // namespace Serein::Media
