#include "serein/hooks/ghost.h"

#include "data/data_peer.h"
#include "history/history.h"
#include "main/main_session.h"
#include "serein/core/options.h"
#include "serein/features/ghost/model/exceptions.h"
#include "serein/schema/gen/settings/ghost.h"

namespace Serein::Hooks {

bool AllowReadReceiptIn(gsl::not_null<History*> history) {
	const auto session = &history->session();
	return AllowReadReceipt(session)
		|| Serein::Ghost::HasException(
			ForAccount(session).Get(Serein::Ghost::kReadReceiptExceptions),
			history->peer->id.value);
}

bool ReadInboxLocally(
		gsl::not_null<History*> history,
		MsgId tillId,
		std::optional<int> stillUnread) {
	if (AllowReadReceiptIn(history)) {
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
