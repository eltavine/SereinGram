#pragma once

#include <rpl/producer.h>

namespace Serein::Compose {

[[nodiscard]] rpl::producer<> ButtonsChanged();

} // namespace Serein::Compose
