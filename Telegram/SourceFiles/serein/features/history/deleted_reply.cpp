#include "serein/hooks/history.h"

#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/model/reply_quote.h"
#include "data/data_msg_id.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"

namespace Serein::Hooks {

void QuoteDeletedReply(
		gsl::not_null<::History*> history,
		FullReplyTo &replyTo,
		TextWithEntities &text) {
	const auto item = replyTo.messageId
		? history->owner().message(replyTo.messageId)
		: nullptr;
	if (!item || !HistoryFeature::DeletedInPlace(item)) {
		return;
	}
	auto quote = replyTo.quote.empty()
		? item->originalText()
		: replyTo.quote;
	if (quote.empty()) {
		quote = item->notificationText();
	}
	const auto root = replyTo.topicRootId;
	replyTo = FullReplyTo{
		.messageId = (root
			? FullMsgId(history->peer->id, root)
			: FullMsgId()),
		.topicRootId = root,
		.monoforumPeerId = replyTo.monoforumPeerId,
	};
	text = HistoryFeature::QuoteDeletedMessage(quote, text);
}

} // namespace Serein::Hooks
