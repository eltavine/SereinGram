#pragma once

#include <gsl/pointers>

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Serein::Privacy {

template <typename Entry>
void AddSessionDetails(
	gsl::not_null<Ui::VerticalLayout*> container,
	const Entry &entry);

} // namespace Serein::Privacy
