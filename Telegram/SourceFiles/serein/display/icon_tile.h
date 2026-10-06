#pragma once

#include "ui/style/style_core_types.h"

#include <gsl/pointers>

namespace Ui {
class RpWidget;
} // namespace Ui

namespace Serein::Display {

void AddIconTile(
	gsl::not_null<Ui::RpWidget*> row,
	const style::icon &icon,
	const style::color &tile);

} // namespace Serein::Display
