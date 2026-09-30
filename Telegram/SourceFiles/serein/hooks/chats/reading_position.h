#pragma once

#include <gsl/pointers>

struct MsgId;

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks {

[[nodiscard]] MsgId ReadingPosition(
	gsl::not_null<Main::Session*> session,
	unsigned long long peerId,
	MsgId showAtMsgId,
	bool reopened);

} // namespace Serein::Hooks
