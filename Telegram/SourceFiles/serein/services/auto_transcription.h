#pragma once

#include <gsl/pointers>

namespace Main {
class Session;
} // namespace Main

namespace Serein {

void WatchAutoTranscription(gsl::not_null<Main::Session*> session);

} // namespace Serein
