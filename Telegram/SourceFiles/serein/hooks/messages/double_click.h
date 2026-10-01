#pragma once

#include <gsl/pointers>

class HistoryItem;

namespace Serein::Messages {

[[nodiscard]] bool EditOnDoubleClick(gsl::not_null<HistoryItem*> item);

} // namespace Serein::Messages
