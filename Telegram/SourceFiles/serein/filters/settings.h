#pragma once

#include <gsl/pointers>

namespace Main { class Session; }
namespace Ui { class GenericBox; }

namespace Serein::Filters {

void SettingsBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);

} // namespace Serein::Filters
