#pragma once

#include "serein/schema/gen/config/snapshot.h"

namespace Serein::Snapshot {

[[nodiscard]] SnapshotConfig Defaults();
[[nodiscard]] std::optional<SnapshotConfig> ReadStored(const QByteArray &raw);
[[nodiscard]] bool Validate(const QByteArray &raw);

} // namespace Serein::Snapshot
