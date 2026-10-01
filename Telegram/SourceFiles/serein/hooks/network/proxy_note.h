#pragma once

#include "base/assertion.h"
#include "base/basic_types.h"
#include "ui/text/text_entity.h"

namespace Serein::Hooks::Network {

[[nodiscard]] TextWithEntities ProxyNote(const QString &host, uint32 port);

} // namespace Serein::Hooks::Network
