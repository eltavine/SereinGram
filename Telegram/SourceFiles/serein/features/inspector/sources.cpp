#include "serein/features/inspector/sources.h"

#include "serein/features/inspector/dump.h"
#include "data/data_channel.h"
#include "data/data_chat.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_types.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"
#include "mtproto/sender.h"

namespace Serein::Inspector {
namespace {

[[nodiscard]] SavedProvider &Saved() {
	static auto result = SavedProvider();
	return result;
}

[[nodiscard]] std::vector<FactsProvider> &Contributors() {
	static auto result = std::vector<FactsProvider>();
	return result;
}

[[nodiscard]] Snapshot SavedCopy(
		gsl::not_null<Main::Session*> session,
		FullMsgId id,
		Snapshot fallback) {
	const auto item = session->data().message(id);
	const auto &provider = Saved();
	if (!item || !provider) {
		return fallback;
	}
	auto root = ReadSerialized(provider(item));
	if (!root) {
		return fallback;
	}
	return { .root = std::move(root), .origin = Origin::Saved };
}

[[nodiscard]] std::optional<Node> FindMessage(
		gsl::not_null<Main::Session*> session,
		FullMsgId id,
		const MTPmessages_Messages &result) {
	auto found = std::optional<Node>();
	result.match([](const MTPDmessages_messagesNotModified &) {
	}, [&](const auto &data) {
		session->data().processUsers(data.vusers());
		session->data().processChats(data.vchats());
		for (const auto &message : data.vmessages().v) {
			if (message.type() != mtpc_messageEmpty
				&& IdFromMessage(message) == id.msg
				&& PeerFromMessage(message) == id.peer) {
				found = ReadSerialized(Serialize(message));
			}
		}
	});
	return found;
}

void Finish(Fn<void(Snapshot)> done, std::optional<Node> root) {
	auto snapshot = Snapshot{ .origin = Origin::Server };
	if (root) {
		snapshot.root = std::move(root);
	} else {
		snapshot.origin = Origin::Failed;
	}
	done(std::move(snapshot));
}

} // namespace

void SetSavedProvider(SavedProvider provider) {
	Saved() = std::move(provider);
}

void AddFactsProvider(FactsProvider provider) {
	Contributors().push_back(std::move(provider));
}

ExtraFacts CollectExtraFacts(gsl::not_null<HistoryItem*> item) {
	auto result = ExtraFacts();
	for (const auto &provider : Contributors()) {
		auto facts = provider(item);
		result.insert(
			end(result),
			std::make_move_iterator(begin(facts)),
			std::make_move_iterator(end(facts)));
	}
	return result;
}

void LoadMessage(
		gsl::not_null<HistoryItem*> item,
		MTP::Sender &api,
		Fn<void(Snapshot)> done) {
	const auto session = &item->history()->session();
	const auto id = item->fullId();
	if (!item->isRegular()) {
		done(SavedCopy(session, id, {}));
		return;
	}
	const auto list = MTP_vector<MTPInputMessage>(
		1,
		MTP_inputMessageID(MTP_int(id.msg.bare)));
	const auto received = [=](const MTPmessages_Messages &result) {
		auto root = FindMessage(session, id, result);
		done(root
			? Snapshot{ .root = std::move(root), .origin = Origin::Server }
			: SavedCopy(session, id, {}));
	};
	const auto failed = [=](const MTP::Error &error) {
		done(SavedCopy(session, id, {
			.origin = Origin::Failed,
			.error = error.type(),
		}));
	};
	if (const auto channel = item->history()->peer->asChannel()) {
		api.request(MTPchannels_GetMessages(
			channel->inputChannel(),
			list
		)).done(received).fail(failed).send();
	} else {
		api.request(MTPmessages_GetMessages(
			list
		)).done(received).fail(failed).send();
	}
}

void LoadPeer(
		gsl::not_null<PeerData*> peer,
		MTP::Sender &api,
		Fn<void(Snapshot)> done) {
	const auto failed = [=](const MTP::Error &error) {
		done({ .origin = Origin::Failed, .error = error.type() });
	};
	const auto full = [=](const MTPmessages_ChatFull &result) {
		Finish(done, ReadSerialized(Serialize(result)));
	};
	if (const auto user = peer->asUser()) {
		api.request(MTPusers_GetFullUser(
			user->inputUser()
		)).done([=](const MTPusers_UserFull &result) {
			Finish(done, ReadSerialized(Serialize(result)));
		}).fail(failed).send();
	} else if (const auto channel = peer->asChannel()) {
		api.request(MTPchannels_GetFullChannel(
			channel->inputChannel()
		)).done(full).fail(failed).send();
	} else if (const auto chat = peer->asChat()) {
		api.request(MTPmessages_GetFullChat(
			chat->inputChat()
		)).done(full).fail(failed).send();
	} else {
		done({});
	}
}

} // namespace Serein::Inspector
