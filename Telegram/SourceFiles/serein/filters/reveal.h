#pragma once

#include "base/basic_types.h"

class History;

namespace Serein::Filters {

[[nodiscard]] bool Revealed(not_null<::History*> history);
void ToggleRevealed(not_null<::History*> history);

} // namespace Serein::Filters
