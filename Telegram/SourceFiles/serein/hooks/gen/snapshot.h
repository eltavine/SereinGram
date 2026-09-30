// Generated from proto/serein/settings/v1/snapshot.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <rpl/producer.h>

namespace Serein::Hooks::Snapshot {

[[nodiscard]] QByteArray Settings();
[[nodiscard]] rpl::producer<QByteArray> SettingsValue();

} // namespace Serein::Hooks::Snapshot
