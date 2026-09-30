#include "serein/hooks/privacy/profile.h"

#include "serein/privacy/options.h"
#include "data/data_changes.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "main/main_session.h"
#include "ui/image/image_location.h"

namespace Serein::Privacy {

rpl::producer<TextWithEntities> ProfileIdValue(not_null<PeerData*> peer) {
	return ForDevice().Value(kProfileIdFormat) | rpl::map([=](int format) {
		if (!format) {
			return TextWithEntities();
		}
		const auto raw = peer->id.value & PeerId::kChatTypeMask;
		auto result = QString::number(raw);
		if (format == 1) {
			if (peer->isChat()) {
				result.prepend('-');
			} else if (peer->isChannel()) {
				result.prepend(u"-100"_q);
			}
		}
		return TextWithEntities{ result };
	});
}

rpl::producer<TextWithEntities> ProfileDcValue(not_null<PeerData*> peer) {
	return rpl::combine(
		ForDevice().Value(kShowProfileDc),
		peer->session().changes().peerFlagsValue(
			peer, Data::PeerUpdate::Flag::Photo)
	) | rpl::map([=](bool show, const auto &) {
		if (!show) {
			return TextWithEntities();
		}
		const auto location = peer->userpicLocation();
		const auto file = std::get_if<StorageFileLocation>(
			&location.file().data);
		return (file && file->dcId() > 0)
			? TextWithEntities{ QString::number(file->dcId()) }
			: TextWithEntities();
	});
}

} // namespace Serein::Privacy
