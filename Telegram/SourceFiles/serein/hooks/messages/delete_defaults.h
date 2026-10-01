#pragma once

#include "serein/hooks/gen/messages.h"

namespace Serein::Messages {

template <typename Options>
[[nodiscard]] Options ModerateDefaults() {
	return Options{
		.reportSpam = Hooks::Messages::ModerateReportSpam(),
		.deleteAll = Hooks::Messages::ModerateDeleteAll(),
		.banUser = Hooks::Messages::ModerateBan(),
	};
}

[[nodiscard]] inline bool RevokeChatByDefault(bool privateChat) {
	return privateChat && Hooks::Messages::RevokePrivateChatDeletion();
}

} // namespace Serein::Messages
