#include "serein/app/modules.h"

#include "serein/hooks/history.h"
#include "serein/hooks/interface/roundness.h"
#include "serein/hooks/interface/text.h"

#include <array>

namespace Serein::App {
namespace {

constexpr auto kModules = std::array{
	Module{ "interface.roundness", Interface::StartRoundness },
	Module{ "interface.text", Interface::StartUiText },
	Module{ "history.retention", nullptr, Hooks::PruneHistory },
};

} // namespace

std::span<const Module> Modules() {
	return kModules;
}

} // namespace Serein::App
