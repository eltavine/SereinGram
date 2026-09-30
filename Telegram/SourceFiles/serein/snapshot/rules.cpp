#include "serein/schema/gen/config/snapshot.h"
#include "serein/schema/gen/settings/snapshot.h"

namespace Serein::Snapshot {

bool Validate(const QByteArray &raw) {
	return raw.isEmpty() || ParseSnapshotConfig(raw).has_value();
}

} // namespace Serein::Snapshot
