#pragma once

#include "settings/settings_builder.h"

namespace Serein {

void AddProxySubscriptionRow(::Settings::Builder::SectionBuilder &builder);
void AddProxyToolRows(::Settings::Builder::SectionBuilder &builder);
void AddCustomDohRow(::Settings::Builder::SectionBuilder &builder);
void AddDatacenterStatusRow(::Settings::Builder::SectionBuilder &builder);

} // namespace Serein
