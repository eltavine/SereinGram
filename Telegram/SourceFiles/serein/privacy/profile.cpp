#include "serein/hooks/privacy/profile.h"

#include "serein/features/regdate/model/estimate.h"
#include "serein/privacy/options.h"
#include "serein/display/peer_id.h"
#include "data/data_changes.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/image/image_location.h"

#include <QtCore/QFile>

namespace Serein::Privacy {
namespace {

[[nodiscard]] rpl::producer<TextWithEntities> IdValue(
		not_null<PeerData*> peer) {
	return ForDevice().Value(kProfileIdFormat) | rpl::map([=](int format) {
		if (!format) {
			return TextWithEntities();
		}
		return TextWithEntities{ Display::PeerIdText(peer, format == 1) };
	});
}

[[nodiscard]] rpl::producer<TextWithEntities> DcValue(
		not_null<PeerData*> peer) {
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

[[nodiscard]] const std::vector<RegistrationDate::Point> &Points() {
	static const auto result = [] {
		auto file = QFile(u":/serein/regdate_points.json"_q);
		return file.open(QIODevice::ReadOnly)
			? RegistrationDate::ParsePoints(file.readAll())
			: std::vector<RegistrationDate::Point>();
	}();
	return result;
}

[[nodiscard]] rpl::producer<TextWithEntities> RegistrationValue(
		not_null<PeerData*> peer) {
	const auto user = peer->asUser();
	return ForDevice().Value(kShowRegistrationDate) | rpl::map([=](bool show) {
		const auto estimate = (show && user)
			? RegistrationDate::EstimateFor(
				Points(),
				peerToUser(user->id).bare)
			: std::nullopt;
		if (!estimate) {
			return TextWithEntities();
		}
		const auto when = langMonthOfYearFull(
			estimate->date.month(),
			estimate->date.year());
		return TextWithEntities{ estimate->lowerBound
			? tr::lng_serein_profile_registered_after(tr::now, lt_date, when)
			: tr::lng_serein_profile_registered_about(tr::now, lt_date, when) };
	});
}

[[nodiscard]] rpl::producer<TextWithEntities> ContactValue(
		not_null<PeerData*> peer) {
	const auto user = peer->asUser();
	if (!user) {
		return rpl::single(TextWithEntities());
	}
	return rpl::combine(
		ForDevice().Value(kShowContactStatus),
		user->flagsValue()
	) | rpl::map([=](bool show, const auto &) {
		using Flag = UserDataFlag;
		const auto flags = user->flags();
		if (!show || !(flags & Flag::Contact)) {
			return TextWithEntities();
		}
		return TextWithEntities{ (flags & Flag::MutualContact)
			? tr::lng_serein_profile_contact_mutual(tr::now)
			: tr::lng_serein_profile_contact_one_way(tr::now) };
	});
}

} // namespace

void FillProfileRows(not_null<PeerData*> peer, const ProfileRow &add) {
	add(tr::lng_serein_profile_id(), IdValue(peer));
	add(tr::lng_serein_profile_dc(), DcValue(peer));
	add(tr::lng_serein_profile_registered(), RegistrationValue(peer));
	add(tr::lng_serein_profile_contact(), ContactValue(peer));
}

} // namespace Serein::Privacy
