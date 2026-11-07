#include "serein/hooks/history.h"

#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/model/reply_quote.h"
#include "serein/hooks/gen/history.h"
#include "api/api_common.h"
#include "api/api_sending.h"
#include "api/api_text_entities.h"
#include "apiwrap.h"
#include "base/flat_set.h"
#include "base/random.h"
#include "data/data_chat_participant_status.h"
#include "data/data_forum_topic.h"
#include "data/data_histories.h"
#include "data/data_msg_id.h"
#include "data/data_peer.h"
#include "data/data_premium_limits.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_helpers.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "storage/localimageloader.h"

#include <map>
#include <vector>

namespace Serein::Hooks {
namespace {

struct PendingQuote {
	FullMsgId target;
	FullMsgId quote;
	std::vector<FullMsgId> uploads;
};

using PendingQuotes = std::map<Main::Session*, std::vector<PendingQuote>>;

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

[[nodiscard]] PendingQuotes &Pending() {
	static auto result = PendingQuotes();
	return result;
}

void Forget(gsl::not_null<Main::Session*> session, FullMsgId id) {
	const auto i = Pending().find(session.get());
	if (i == end(Pending())) {
		return;
	}
	auto orphans = std::vector<FullMsgId>();
	auto &list = i->second;
	for (auto j = begin(list); j != end(list);) {
		auto &uploads = j->uploads;
		uploads.erase(ranges::remove(uploads, id), end(uploads));
		if (j->quote == id) {
			j = list.erase(j);
		} else if (uploads.empty()) {
			orphans.push_back(j->quote);
			j = list.erase(j);
		} else {
			++j;
		}
	}
	for (const auto &quote : orphans) {
		crl::on_main(session, [=] {
			if (const auto item = session->data().message(quote)) {
				item->destroy();
			}
		});
	}
}

[[nodiscard]] std::vector<PendingQuote> &PendingFor(
		gsl::not_null<Main::Session*> session) {
	const auto raw = session.get();
	const auto [i, fresh] = Pending().try_emplace(raw);
	if (fresh) {
		session->data().itemRemoved(
		) | rpl::on_next([=](gsl::not_null<const HistoryItem*> item) {
			Forget(raw, item->fullId());
		}, session->lifetime());
		session->lifetime().add([=] { Pending().erase(raw); });
	}
	return i->second;
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

[[nodiscard]] bool Quoting(gsl::not_null<::History*> history) {
	return HistorySettings::HistoryQuoteDeletedReplies(&history->session());
}

// Slow mode refuses a second message and paid chats would charge for it.
[[nodiscard]] bool SeparateQuoteAllowed(
		gsl::not_null<::History*> history,
		const FullReplyTo &replyTo,
		const Api::SendOptions &options) {
	const auto peer = history->peer;
	const auto topic = peer->forumTopicFor(replyTo.topicRootId
		? replyTo.topicRootId
		: Data::ForumTopic::kGeneralId);
	return Quoting(history)
		&& (topic ? Data::CanSendTexts(topic) : Data::CanSendTexts(peer))
		&& !options.suggest
		&& !options.shortcutId
		&& !peer->starsPerMessageChecked()
		&& !peer->slowmodeApplied();
}

[[nodiscard]] QString Author(gsl::not_null<HistoryItem*> item) {
	const auto peer = item->history()->peer;
	if (!peer->isChat() && !peer->isMegagroup()) {
		return QString();
	}
	const auto from = item->displayFrom();
	const auto sender = from
		? gsl::not_null<PeerData*>(from)
		: item->author();
	if (const auto user = sender->asUser(); user && user->isContact()) {
		const auto username = user->username();
		return username.isEmpty() ? QString() : (u"@"_q + username);
	}
	return sender->name();
}

[[nodiscard]] TextWithEntities BuildQuote(
		gsl::not_null<HistoryItem*> item,
		const FullReplyTo &replyTo,
		const TextWithEntities &text) {
	auto quoted = replyTo.quote.empty()
		? item->originalText()
		: replyTo.quote;
	if (quoted.empty()) {
		quoted = item->notificationText();
	}
	const auto session = &item->history()->session();
	return HistoryFeature::QuoteDeletedMessage(
		{ .author = Author(item), .text = std::move(quoted) },
		text,
		Data::PremiumLimits(session).messageLengthCurrent());
}

[[nodiscard]] HistoryItem *AddQuote(
		gsl::not_null<::History*> history,
		const FullReplyTo &replyTo,
		const Api::SendOptions &options,
		const TextWithEntities &text) {
	if (text.empty()) {
		return nullptr;
	}
	const auto peer = history->peer;
	auto action = Api::SendAction(history, options);
	action.replyTo = replyTo;
	auto flags = NewMessageFlags(peer);
	if (replyTo) {
		flags |= MessageFlag::HasReplyInfo;
	}
	Api::FillMessagePostFlags(action, peer, flags);
	if (options.scheduled) {
		flags |= MessageFlag::IsOrWasScheduled;
	}
	return history->addNewLocalMessage({
		.id = history->owner().nextLocalMessageId(),
		.flags = flags,
		.from = NewMessageFromId(action),
		.replyTo = replyTo,
		.date = NewMessageDate(options),
		.scheduleRepeatPeriod = options.scheduleRepeatPeriod,
		.postAuthor = NewMessagePostAuthor(action),
	}, text, MTP_messageMediaEmpty());
}

void SendQuote(
		gsl::not_null<HistoryItem*> quote,
		const Api::SendOptions &options) {
	const auto history = quote->history();
	const auto peer = history->peer;
	const auto session = &history->session();
	const auto id = quote->fullId();
	const auto replyTo = quote->replyTo();
	const auto text = quote->originalText();
	const auto randomId = base::RandomValue<uint64>();
	session->data().registerMessageRandomId(randomId, id);
	session->data().registerMessageSentData(randomId, peer->id, text.text);
	const auto entities = Api::EntitiesToMTP(
		session,
		text.entities,
		Api::ConvertOption::SkipLocal);
	const auto sendAs = options.sendAs;
	const auto repeat = options.scheduled && options.scheduleRepeatPeriod;
	const auto effectId = uint64(0);
	const auto starsPaid = int64(0);
	using Flag = MTPmessages_SendMessage::Flag;
	const auto flags = Flag(0)
		| Flag::f_no_webpage
		| (replyTo ? Flag::f_reply_to : Flag(0))
		| (ShouldSendSilent(peer, options) ? Flag::f_silent : Flag(0))
		| (entities.v.isEmpty() ? Flag(0) : Flag::f_entities)
		| (options.scheduled ? Flag::f_schedule_date : Flag(0))
		| (repeat ? Flag::f_schedule_repeat_period : Flag(0))
		| (sendAs ? Flag::f_send_as : Flag(0));
	history->owner().histories().sendPreparedMessage(
		history,
		replyTo,
		randomId,
		Data::Histories::PrepareMessage<MTPmessages_SendMessage>(
			MTP_flags(flags),
			peer->input(),
			Data::Histories::ReplyToPlaceholder(),
			MTP_string(text.text),
			MTP_long(randomId),
			MTPReplyMarkup(),
			entities,
			MTP_int(options.scheduled),
			MTP_int(options.scheduleRepeatPeriod),
			(sendAs ? sendAs->input() : MTP_inputPeerEmpty()),
			MTPInputQuickReplyShortcut(),
			MTP_long(effectId),
			MTP_long(starsPaid),
			MTPSuggestedPost(),
			MTPInputRichMessage()),
		[](const MTPUpdates &, const MTP::Response &) {
		},
		[=](const MTP::Error &error, const MTP::Response &) {
			session->api().sendMessageFail(error, peer, randomId, id);
		});
}

} // namespace

void QuoteDeletedReply(Api::SendAction &action, TextWithEntities &text) {
	const auto history = action.history;
	const auto item = DeletedTarget(history, action.replyTo);
	if (!item) {
		return;
	}
	if (Quoting(history)
		&& !action.options.suggest
		&& !QuotedNow().contains(item->fullId())) {
		text = BuildQuote(item, action.replyTo, text);
		RememberQuoted(item->fullId());
	}
	action.replyTo = WithoutTarget(history, action.replyTo);
}

FullReplyTo DetachDeletedReply(const Api::SendAction &action) {
	const auto history = action.history;
	const auto item = (action.replaceMediaOf || action.options.welcomeTemplate)
		? nullptr
		: DeletedTarget(history, action.replyTo);
	if (!item) {
		return action.replyTo;
	}
	const auto result = WithoutTarget(history, action.replyTo);
	if (!QuotedNow().contains(item->fullId())
		&& SeparateQuoteAllowed(history, result, action.options)) {
		const auto quote = AddQuote(
			history,
			result,
			action.options,
			BuildQuote(item, action.replyTo, {}));
		if (quote) {
			SendQuote(quote, action.options);
			history->owner().sendHistoryChangeNotifications();
		}
	}
	RememberQuoted(item->fullId());
	return result;
}

FullReplyTo UploadReplyTo(const Api::SendAction &action) {
	const auto history = action.history;
	const auto item = action.replaceMediaOf
		? nullptr
		: DeletedTarget(history, action.replyTo);
	return (item && QuotedNow().contains(item->fullId()))
		? WithoutTarget(history, action.replyTo)
		: action.replyTo;
}

void OnUploadPrepared(
		gsl::not_null<::History*> history,
		FileLoadTo &to,
		FullMsgId localId) {
	const auto item = (to.replaceMediaOf || to.options.welcomeTemplate)
		? nullptr
		: DeletedTarget(history, to.replyTo);
	if (!item) {
		return;
	}
	const auto replyTo = std::exchange(
		to.replyTo,
		WithoutTarget(history, to.replyTo));
	if (!SeparateQuoteAllowed(history, to.replyTo, to.options)) {
		return;
	}
	auto &pending = PendingFor(&history->session());
	const auto target = item->fullId();
	const auto i = ranges::find(pending, target, &PendingQuote::target);
	if (i != end(pending)) {
		i->uploads.push_back(localId);
		return;
	}
	const auto quote = AddQuote(
		history,
		to.replyTo,
		to.options,
		BuildQuote(item, replyTo, {}));
	if (quote) {
		pending.push_back({
			.target = target,
			.quote = quote->fullId(),
			.uploads = { localId },
		});
	}
}

void OnMediaSending(
		gsl::not_null<HistoryItem*> item,
		const Api::SendOptions &options) {
	const auto history = item->history();
	const auto i = Pending().find(&history->session());
	if (i == end(Pending())) {
		return;
	}
	auto &list = i->second;
	const auto j = ranges::find_if(list, [&](const PendingQuote &entry) {
		return ranges::contains(entry.uploads, item->fullId());
	});
	if (j == end(list)) {
		return;
	}
	const auto id = j->quote;
	list.erase(j);
	if (const auto quote = history->owner().message(id)) {
		if (SeparateQuoteAllowed(history, quote->replyTo(), options)) {
			SendQuote(quote, options);
		} else {
			quote->destroy();
		}
	}
}

std::optional<TextWithEntities> DeletedReplyPreviewName(
		gsl::not_null<HistoryItem*> to) {
	if (!HistoryFeature::DeletedInPlace(to)) {
		return std::nullopt;
	}
	const auto from = to->displayFrom();
	const auto sender = from
		? gsl::not_null<PeerData*>(from)
		: to->author();
	const auto phrase = Quoting(to->history())
		? tr::lng_serein_reply_deleted_quote
		: tr::lng_serein_reply_deleted_plain;
	return phrase(
		tr::now,
		lt_name,
		TextWithEntities{ sender->name() },
		tr::marked);
}

} // namespace Serein::Hooks
