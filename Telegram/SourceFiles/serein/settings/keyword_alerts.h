#pragma once

namespace Settings::Builder {
class SectionBuilder;
} // namespace Settings::Builder

namespace Serein::Filters {

void AddKeywordAlerts(::Settings::Builder::SectionBuilder &builder);

} // namespace Serein::Filters
