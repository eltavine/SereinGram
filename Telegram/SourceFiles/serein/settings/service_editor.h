#pragma once

#include "serein/hooks/services/model.h"

#include <gsl/pointers>

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Serein {

[[nodiscard]] bool ServicesUnchanged(
	gsl::not_null<Ui::GenericBox*> box,
	const ServicesConfig &expected);
void ServicesBox(gsl::not_null<Ui::GenericBox*> box, ServicesConfig initial);

} // namespace Serein
