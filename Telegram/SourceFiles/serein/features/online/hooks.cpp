#include "serein/hooks/online.h"

#include "serein/core/options.h"
#include "serein/features/online/status.h"
#include "serein/schema/gen/settings/interface.h"
#include "base/unixtime.h"
#include "core/click_handler_types.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "dialogs/dialogs_entry.h"
#include "history/history.h"
#include "main/main_session.h"
#include "ui/painter.h"
#include "styles/style_chat.h"
#include "styles/style_serein.h"

namespace Serein::Hooks::Online {
namespace {

[[nodiscard]] bool Enabled(const Serein::Option<bool> &option) {
	return ForDevice().Get(option);
}

[[nodiscard]] bool Detailed() {
	return Enabled(Serein::Interface::kOnlineStatusDetailed);
}

[[nodiscard]] UserData *StatusUser(PeerData *peer) {
	const auto user = peer ? peer->asUser() : nullptr;
	return (user && !user->isBot() && !user->isSelf() && !user->isServiceUser())
		? user
		: nullptr;
}

} // namespace

QString RowPrefix(gsl::not_null<const Dialogs::Entry*> entry) {
	if (!Enabled(Serein::Interface::kOnlineStatusInChats)) {
		return QString();
	}
	const auto history = entry->asHistory();
	const auto user = history ? StatusUser(history->peer) : nullptr;
	if (!user) {
		return QString();
	}
	const auto now = base::unixtime::now();
	const auto text = Detailed()
		? Serein::Online::ExactText(user->lastseen(), now, false)
		: Serein::Online::CompactText(user->lastseen(), now);
	return text.isEmpty() ? QString() : (text + u" \u00B7 "_q);
}

std::optional<QString> StatusText(
		const Data::LastseenStatus &status,
		TimeId now) {
	if (!Enabled(Serein::Interface::kOnlineStatusInHeader)) {
		return std::nullopt;
	}
	const auto till = status.onlineTill();
	if (till <= 0 || till > now) {
		return std::nullopt;
	}
	return Serein::Online::LastSeenLine(status, now, Detailed());
}

QString LinkTooltip(
		gsl::not_null<Main::Session*> session,
		const std::shared_ptr<ClickHandler> &link) {
	const auto fallback = link ? link->tooltip() : QString();
	if (!link
		|| !fallback.isEmpty()
		|| !Detailed()
		|| !Enabled(Serein::Interface::kOnlineStatusOnSenders)) {
		return fallback;
	}
	const auto value = link->property(kPeerLinkPeerIdProperty);
	if (!value.isValid()) {
		return fallback;
	}
	const auto peer = session->data().peerLoaded(
		PeerId(PeerIdHelper(BareId(value.toULongLong()))));
	const auto user = StatusUser(peer);
	return user
		? Serein::Online::LastSeenLine(
			user->lastseen(),
			base::unixtime::now(),
			true)
		: fallback;
}

void PaintSender(
		QPainter &p,
		gsl::not_null<PeerData*> from,
		int top,
		int outerWidth) {
	if (!Enabled(Serein::Interface::kOnlineStatusOnSenders)) {
		return;
	}
	const auto user = StatusUser(from);
	if (!user || !user->lastseen().isOnline(base::unixtime::now())) {
		return;
	}
	const auto size = st::sereinSenderOnlineSize;
	const auto left = st::historyPhotoLeft + st::msgPhotoSize - size;
	const auto y = top + st::msgPhotoSize - size;
	auto hq = PainterHighQualityEnabler(p);
	auto pen = st::windowBg->p;
	pen.setWidthF(st::sereinSenderOnlineStroke);
	p.setPen(pen);
	p.setBrush(st::dialogsOnlineBadgeFg->b);
	p.drawEllipse(style::rtlrect(left, y, size, size, outerWidth));
}

} // namespace Serein::Hooks::Online
