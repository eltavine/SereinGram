#pragma once

#include "lang/lang_keys.h"
#include "serein/core/options.h"
#include "settings/settings_builder.h"

#include <span>

namespace Serein {

struct ToggleRow {
	const Option<bool> *option = nullptr;
	tr::phrase<> title;
	QString id;
	QStringList keywords;
};

void AddToggle(
	::Settings::Builder::SectionBuilder &builder,
	const ToggleRow &row);
void AddToggles(
	::Settings::Builder::SectionBuilder &builder,
	std::span<const ToggleRow> rows);

} // namespace Serein
