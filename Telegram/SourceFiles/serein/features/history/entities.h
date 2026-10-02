#pragma once

#include "base/assertion.h"
#include "ui/text/text_entity.h"

#include <optional>

namespace Serein::HistoryFeature {

[[nodiscard]] QString EntityName(EntityType type);
[[nodiscard]] std::optional<EntityType> EntityTypeFromName(const QString &name);

} // namespace Serein::HistoryFeature
