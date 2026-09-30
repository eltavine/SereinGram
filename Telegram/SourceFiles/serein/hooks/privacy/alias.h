#pragma once

#include "base/basic_types.h"

#include <memory>

class PeerData;
namespace Main { class SessionShow; }

namespace Serein::Privacy {

[[nodiscard]] const QString &Alias(not_null<const PeerData*> peer);
[[nodiscard]] const QString &DisplayName(not_null<const PeerData*> peer);
void ShowAlias(std::shared_ptr<Main::SessionShow> show, not_null<PeerData*> peer);

} // namespace Serein::Privacy
