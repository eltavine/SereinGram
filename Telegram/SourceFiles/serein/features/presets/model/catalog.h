#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <optional>
#include <vector>

namespace Serein::Presets {

[[nodiscard]] bool ValidPresetId(const QString &id);

// Preset ids in display order, or nothing when the catalog is malformed.
[[nodiscard]] std::optional<std::vector<QString>> ParseCatalog(
	const QByteArray &json);

} // namespace Serein::Presets
