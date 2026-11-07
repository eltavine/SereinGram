#pragma once

#include "settings/settings_common.h"

namespace Settings::Builder {
class SectionBuilder;
} // namespace Settings::Builder

namespace Serein {

[[nodiscard]] Settings::Type ConfigId();
void AddConfigButton(::Settings::Builder::SectionBuilder &builder);

} // namespace Serein
