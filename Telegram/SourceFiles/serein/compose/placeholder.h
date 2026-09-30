#pragma once

#include "base/basic_types.h"
#include "rpl/rpl.h"

class PeerData;

namespace Serein::Compose {

[[nodiscard]] rpl::producer<QString> InputPlaceholder(
	not_null<PeerData*> peer);

} // namespace Serein::Compose
