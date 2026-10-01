#pragma once

#include "base/basic_types.h"
#include "data/data_peer_id.h"

#include <gsl/pointers>

class UserData;

namespace Main {
class Session;
} // namespace Main

namespace Serein {

[[nodiscard]] bool HasUserLookup();
void LookupUser(
	gsl::not_null<Main::Session*> session,
	UserId id,
	Fn<void(UserData*)> done);

} // namespace Serein
