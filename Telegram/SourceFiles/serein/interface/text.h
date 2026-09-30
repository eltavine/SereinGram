#pragma once

#include <QtCore/QString>

namespace Serein::Interface {

[[nodiscard]] QString HalfwidthPunctuation(QString value);
[[nodiscard]] QString UiText(QString value);
void StartUiText();

} // namespace Serein::Interface
