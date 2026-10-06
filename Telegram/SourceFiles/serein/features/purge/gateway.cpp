#include "serein/features/purge/gateway.h"

#include "apiwrap.h"
#include "data/data_histories.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_types.h"
#include "data/data_user.h"
#include "history/history_item.h"
#include "main/main_session.h"
#include "main/session/send_as_peers.h"

namespace Serein::Purge {
namespace {

constexpr auto kPerPage = 100;

[[nodiscard]] MTPInputPeer InputFor(gsl::not_null<PeerData*> identity) {
	return identity->isSelf() ? MTP_inputPeerSelf() : identity->input();
}

[[nodiscard]] MTPmessages_Search SearchRequest(
		gsl::not_null<PeerData*> chat,
		gsl::not_null<PeerData*> identity,
		qint64 before,
		qint64 offsetId,
		int limit) {
	using Flag = MTPmessages_Search::Flag;
	return MTPmessages_Search(
		MTP_flags(Flag::f_from_id),
		chat->input(),
		MTP_string(),
		InputFor(identity),
		MTP_inputPeerEmpty(), // saved_peer_id
		MTPVector<MTPReaction>(), // saved_reaction
		MTP_int(0), // top_msg_id
		MTP_inputMessagesFilterEmpty(),
		MTP_int(0), // min_date
		MTP_int(before),
		MTP_int(offsetId),
		MTP_int(0), // add_offset
		MTP_int(limit),
		MTP_int(0), // max_id
		MTP_int(0), // min_id
		MTP_long(0)); // hash
}

class MtpGateway final : public Gateway {
public:
	MtpGateway(gsl::not_null<PeerData*> chat, Identities identities)
	: _chat(chat)
	, _identities(std::move(identities)) {
	}

	void page(
			int identity,
			qint64 before,
			qint64 offsetId,
			Fn<void(std::vector<qint64>)> done,
			Fn<void(QString)> fail) override {
		const auto chat = _chat;
		const auto from = _identities[identity];
		chat->session().api().request(SearchRequest(
			chat,
			from,
			before,
			offsetId,
			kPerPage
		)).done([=](const MTPmessages_Messages &result) {
			done(Collect(chat, from, result));
		}).fail([=](const MTP::Error &error) {
			fail(error.type());
		}).send();
	}

	void erase(int, const std::vector<qint64> &ids) override {
		auto list = MessageIdsList();
		list.reserve(ids.size());
		for (const auto id : ids) {
			list.push_back(FullMsgId(_chat->id, MsgId(id)));
		}
		auto &owner = _chat->owner();
		owner.histories().deleteMessages(list, true);
		owner.sendHistoryChangeNotifications();
	}

private:
	[[nodiscard]] static std::vector<qint64> Collect(
			gsl::not_null<PeerData*> chat,
			gsl::not_null<PeerData*> from,
			const MTPmessages_Messages &result) {
		auto &owner = chat->owner();
		auto ids = std::vector<qint64>();
		result.match([](const MTPDmessages_messagesNotModified &) {
		}, [&](const auto &data) {
			owner.processUsers(data.vusers());
			owner.processChats(data.vchats());
			for (const auto &message : data.vmessages().v) {
				if (PeerFromMessage(message) != chat->id
					|| !DateFromMessage(message)) {
					continue;
				}
				const auto item = owner.addNewMessage(
					message,
					MessageFlags(),
					NewMessageType::Existing);
				if (item && item->from() == from) {
					ids.push_back(item->id.bare);
				}
			}
		});
		return ids;
	}

	const gsl::not_null<PeerData*> _chat;
	const Identities _identities;

};

} // namespace

Identities IdentitiesFor(gsl::not_null<PeerData*> chat) {
	const auto session = &chat->session();
	const auto self = gsl::not_null<PeerData*>(session->user());
	auto result = Identities{ self };
	const auto key = Main::SendAsKey(chat);
	for (const auto &entry : session->sendAsPeers().list(key)) {
		const auto peer = entry.peer;
		if (peer != self && peer != chat && peer->isChannel()) {
			result.push_back(peer);
		}
	}
	return result;
}

std::shared_ptr<Gateway> MakeGateway(
		gsl::not_null<PeerData*> chat,
		Identities identities) {
	return std::make_shared<MtpGateway>(chat, std::move(identities));
}

void CountMessages(
		gsl::not_null<PeerData*> chat,
		gsl::not_null<PeerData*> identity,
		qint64 before,
		Fn<void(int)> done) {
	chat->session().api().request(SearchRequest(
		chat,
		identity,
		before,
		0,
		1
	)).done([=](const MTPmessages_Messages &result) {
		done(result.match([](const MTPDmessages_messages &data) {
			return int(data.vmessages().v.size());
		}, [](const auto &data) {
			return data.vcount().v;
		}));
	}).fail([=](const MTP::Error &) {
		done(-1);
	}).send();
}

} // namespace Serein::Purge
