#include "serein/hooks/ghost.h"

#include "history/history.h"
#include "main/main_session.h"

namespace Serein::Hooks {

bool ReadInboxLocally(
		gsl::not_null<History*> history,
		MsgId tillId,
		std::optional<int> stillUnread) {
	if (AllowReadReceipt(&history->session())) {
		return false;
	}
	history->setInboxReadTill(tillId);
	if (stillUnread) {
		history->setUnreadCount(*stillUnread);
	}
	history->updateChatListEntry();
	return true;
}

} // namespace Serein::Hooks
