#pragma once

#include "serein/features/inspector/model/tl_tree.h"

#include <QtCore/QByteArray>

namespace Serein::Inspector {

[[nodiscard]] QString RenderText(const Node &root);
[[nodiscard]] QByteArray RenderJson(const Node &root);
[[nodiscard]] QString Humanize(const QString &name);

} // namespace Serein::Inspector
