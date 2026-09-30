// Generated from proto/serein/settings/v1/snapshot.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/snapshot.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/snapshot.h"

namespace Serein::Hooks::Snapshot {

QByteArray Settings() {
	return ForDevice().Get(Serein::Snapshot::kSettings);
}

rpl::producer<QByteArray> SettingsValue() {
	return ForDevice().Value(Serein::Snapshot::kSettings);
}

} // namespace Serein::Hooks::Snapshot
