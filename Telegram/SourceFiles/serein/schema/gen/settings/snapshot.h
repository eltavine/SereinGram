// Generated from proto/serein/settings/v1/snapshot.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Snapshot {

[[nodiscard]] bool Validate(const QByteArray &value);

inline const auto kSettings = Option<QByteArray>{
	"serein.snapshot",
	Scope::Device,
	QByteArray(),
	Category::Menu,
	"lng_serein_snapshot",
	0,
	&Validate };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kSettings));
}

} // namespace Serein::Snapshot
