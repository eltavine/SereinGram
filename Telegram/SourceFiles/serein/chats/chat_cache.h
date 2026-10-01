#pragma once

#include <gsl/pointers>

class History;

namespace Serein::Chats {

[[nodiscard]] int ClearLoadedMediaCache(gsl::not_null<History*> history);

} // namespace Serein::Chats
