#include "serein/hooks/history.h"

#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/model/reply_quote.h"
#include "api/api_common.h"
#include "apiwrap.h"
#include "base/flat_set.h"
#include "data/data_msg_id.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

namespace Serein::Hooks {
namespace {

[[nodiscard]] base::flat_set<FullMsgId> &QuotedNow() {
	static auto result = base::flat_set<FullMsgId>();
	return result;
}

// One send can be split into parts, such as albums, that share a reply.
void RememberQuoted(FullMsgId id) {
	auto &quoted = QuotedNow();
	if (quoted.empty()) {
		crl::on_main([] { QuotedNow().clear(); });
	}
	quoted.emplace(id);
}

[[nodiscard]] HistoryItem *DeletedTarget(
		gsl::not_null<::History*> history,
		const FullReplyTo &replyTo) {
	const auto item = replyTo.messageId
		? history->owner().message(replyTo.messageId)
		: nullptr;
	return (item && HistoryFeature::DeletedInPlace(item)) ? item : nullptr;
}

[[nodiscard]] FullReplyTo WithoutTarget(
		gsl::not_null<::History*> history,
		const FullReplyTo &replyTo) {
	const auto root = replyTo.topicRootId;
	return {
		.messageId = (root
			? FullMsgId(history->peer->id, root)
			: FullMsgId()),
		.topicRootId = root,
		.monoforumPeerId = replyTo.monoforumPeerId,
	};
}

void SendQuote(const Api::SendAction &action) {
	const auto history = action.history;
	auto quote = Api::MessageToSend(Api::SendAction(history, {
		.sendAs = action.options.sendAs,
		.scheduled = action.options.scheduled,
		.scheduleRepeatPeriod = action.options.scheduleRepeatPeriod,
		.shortcutId = action.options.shortcutId,
		.silent = action.options.silent,
	}));
	quote.action.replyTo = action.replyTo;
	quote.action.clearDraft = false;
	quote.webPage.removed = true;
	history->session().api().sendMessage(std::move(quote));
}

} // namespace

void QuoteDeletedReply(
		gsl::not_null<::History*> history,
		FullReplyTo &replyTo,
		TextWithEntities &text) {
	const auto item = DeletedTarget(history, replyTo);
	if (!item) {
		return;
	}
	if (!QuotedNow().contains(item->fullId())) {
		auto quote = replyTo.quote.empty()
			? item->originalText()
			: replyTo.quote;
		if (quote.empty()) {
			quote = item->notificationText();
		}
		text = HistoryFeature::QuoteDeletedMessage(quote, text);
		RememberQuoted(item->fullId());
	}
	replyTo = WithoutTarget(history, replyTo);
}

FullReplyTo DetachDeletedReply(const Api::SendAction &action) {
	const auto history = action.history;
	const auto item = (action.replaceMediaOf || action.options.welcomeTemplate)
		? nullptr
		: DeletedTarget(history, action.replyTo);
	if (!item) {
		return action.replyTo;
	}
	if (!QuotedNow().contains(item->fullId())
		&& !action.options.suggest
		&& !history->peer->starsPerMessageChecked()) {
		SendQuote(action);
	}
	RememberQuoted(item->fullId());
	return WithoutTarget(history, action.replyTo);
}

} // namespace Serein::Hooks
