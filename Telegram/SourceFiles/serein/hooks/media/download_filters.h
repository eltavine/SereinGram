#pragma once

#include "base/basic_types.h"

#include <rpl/lifetime.h>

#include <vector>

class HistoryItem;
struct GlobalMsgId;

namespace Main { class Session; }
namespace Ui::Menu { struct MenuCallback; }

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
