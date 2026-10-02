#pragma once

#include <gsl/pointers>

namespace Main {
class Session;
} // namespace Main

namespace Serein::HistoryFeature {

void WatchRemovedChats(gsl::not_null<Main::Session*> session);

} // namespace Serein::HistoryFeature
