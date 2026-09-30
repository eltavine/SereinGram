#pragma once

#include <gsl/pointers>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Ghost {

void TipHiddenStoryView(gsl::not_null<Main::Session*> session);

} // namespace Serein::Ghost
