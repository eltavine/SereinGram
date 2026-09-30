#include "serein/privacy/peer_id.h"

#include "data/data_peer.h"

namespace Serein::Privacy {

QString PeerIdText(gsl::not_null<PeerData*> peer, bool botApi) {
	auto result = QString::number(peer->id.value & PeerId::kChatTypeMask);
	if (botApi) {
		if (peer->isChat()) {
			result.prepend('-');
		} else if (peer->isChannel()) {
			result.prepend(u"-100"_q);
		}
	}
	return result;
}

} // namespace Serein::Privacy
