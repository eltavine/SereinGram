#pragma once

namespace Settings::Builder {
class SectionBuilder;
} // namespace Settings::Builder

namespace Serein::Interface {

void AddQuietHours(::Settings::Builder::SectionBuilder &builder);

} // namespace Serein::Interface
