#pragma once

#include <gsl/pointers>

class History;

namespace Api {
struct SendOptions;
} // namespace Api

namespace Serein::Hooks {

void ApplySendOptions(
	gsl::not_null<History*> history,
	Api::SendOptions &options);

} // namespace Serein::Hooks
