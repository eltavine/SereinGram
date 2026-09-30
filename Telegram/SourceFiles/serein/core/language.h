#pragma once

#include <QtCore/QString>

namespace Lang {
class Instance;
} // namespace Lang

namespace Serein {

[[nodiscard]] QString LocalizedValue(const Lang::Instance &instance, ushort key);

} // namespace Serein
