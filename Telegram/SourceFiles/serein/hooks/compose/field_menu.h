#pragma once

#include <memory>
#include <gsl/pointers>

namespace Main { class SessionShow; }
namespace Ui { class InputField; }

namespace Serein::Hooks::Compose {

void InstallFieldMenu(
	not_null<Ui::InputField*> field,
	std::shared_ptr<Main::SessionShow> show);

} // namespace Serein::Hooks::Compose
