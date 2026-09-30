#pragma once

#include <rpl/producer.h>

#include <gsl/pointers>

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Serein::Interface {

void StartAppIcon();
[[nodiscard]] rpl::producer<bool> CustomAppIconValue();
void AppIconBox(gsl::not_null<Ui::GenericBox*> box);

} // namespace Serein::Interface
