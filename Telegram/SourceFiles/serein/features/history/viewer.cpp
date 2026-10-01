#include "serein/features/history/viewer.h"

#include "serein/features/history/bubbles.h"

#include "serein/hooks/history.h"
#include "serein/ports/history_store.h"
#include "base/unixtime.h"
#include "core/file_utilities.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/painter.h"
#include "window/window_session_controller.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_serein.h"

#include <QtCore/QFileInfo>
#include <QtGui/QImage>

namespace Serein::HistoryFeature {
namespace {

constexpr auto kDeletedLimit = 100;
constexpr auto kSavedChatsLimit = 200;

[[nodiscard]] std::vector<History::Record> DeletedFor(
		not_null<Main::Session*> session,
		PeerId peer,
		int limit) {
	const auto store = Hooks::HistoryStoreFor(session);
	if (!store) {
		return {};
	}
	return store->deleted({
		.peerId = qint64(peer.value),
		.limit = limit,
	});
}

[[nodiscard]] QString Describe(
		not_null<Main::Session*> session,
		const History::Record &record) {
	auto header = langDateTimeFull(
		base::unixtime::parse(TimeId(record.date)));
	if (record.fromPeerId) {
		const auto from = session->data().peerLoaded(
			PeerId(PeerIdHelper(BareId(record.fromPeerId))));
		if (from) {
			header = from->name() + u", "_q + header;
		}
	}
	const auto body = record.text.isEmpty()
		? record.mediaSummary
		: record.text;
	return header + u"\n"_q + body;
}

[[nodiscard]] QImage CachedPreview(
		not_null<Main::Session*> session,
		const History::Record &record) {
	const auto suffix = QFileInfo(record.cachedMediaName).suffix().toLower();
	if (suffix != u"jpg"_q
		&& suffix != u"jpeg"_q
		&& suffix != u"png"_q
		&& suffix != u"webp"_q) {
		return QImage();
	}
	const auto bytes = Hooks::CachedMediaBytes(session, record);
	return bytes ? QImage::fromData(*bytes) : QImage();
}

void AddPreview(
		not_null<Ui::GenericBox*> box,
		const QImage &image,
		Fn<void()> open) {
	const auto ratio = style::DevicePixelRatio();
	const auto limit = st::sereinHistoryPreviewSize;
	auto size = image.size();
	if (size.width() > limit.width() || size.height() > limit.height()) {
		size = size.scaled(limit, Qt::KeepAspectRatio);
	}
	size = QSize(std::max(size.width(), 1), std::max(size.height(), 1));
	auto scaled = image.scaled(
		size * ratio,
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation);
	scaled.setDevicePixelRatio(ratio);
	const auto preview = QPixmap::fromImage(std::move(scaled));
	const auto row = box->addRow(
		object_ptr<Ui::FixedHeightWidget>(box, size.height()));
	row->paintRequest() | rpl::on_next([=] {
		auto p = QPainter(row);
		p.drawPixmap(0, 0, preview);
	}, row->lifetime());
	const auto button = Ui::CreateChild<Ui::AbstractButton>(row);
	button->setGeometry(QRect(QPoint(), size));
	button->setClickedCallback(std::move(open));
}

} // namespace

bool HasDeletedMessages(
		gsl::not_null<Main::Session*> session,
		gsl::not_null<PeerData*> peer) {
	return !DeletedFor(session, peer->id, 1).empty();
}

void ShowDeletedMessages(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto session = &controller->session();
	const auto records = DeletedFor(session, peer->id, kDeletedLimit);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_menu_deleted_messages());
		if (records.empty()) {
			box->addRow(object_ptr<Ui::FlatLabel>(
				box,
				tr::lng_serein_history_empty(tr::now),
				st::boxLabel));
		} else {
			const auto chat = box->addRow(object_ptr<Ui::LinkButton>(
				box,
				tr::lng_serein_history_show_chat(tr::now)));
			chat->setClickedCallback([=] {
				ShowDeletedBubbles(controller, peer, records);
			});
		}
		for (const auto &record : records) {
			const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
				box,
				Describe(session, record),
				st::boxLabel));
			label->setSelectable(true);
			const auto path = record.localPath;
			if (!path.isEmpty() && QFileInfo::exists(path)) {
				const auto open = box->addRow(object_ptr<Ui::LinkButton>(
					box,
					tr::lng_serein_history_open_file(tr::now)));
				open->setClickedCallback([=] { File::Launch(path); });
			} else if (!record.cachedMediaName.isEmpty()) {
				const auto openMedia = [=] {
					if (!Hooks::OpenCachedMedia(session, record)) {
						box->uiShow()->showToast(
							tr::lng_serein_history_media_missing(tr::now));
					}
				};
				const auto preview = CachedPreview(session, record);
				if (!preview.isNull()) {
					AddPreview(box, preview, openMedia);
				}
				const auto open = box->addRow(object_ptr<Ui::LinkButton>(
					box,
					tr::lng_serein_history_open_media(tr::now)));
				open->setClickedCallback(openMedia);
			}
		}
		if (!records.empty()) {
			box->addLeftButton(tr::lng_serein_history_clear_chat(), [=] {
				ConfirmClearHistory(controller, peer, crl::guard(box, [=] {
					box->closeBox();
				}));
			});
		}
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

void ShowSavedChats(gsl::not_null<Window::SessionController*> controller) {
	const auto session = &controller->session();
	const auto store = Hooks::HistoryStoreFor(session);
	const auto peers = store
		? store->peersWithDeleted(kSavedChatsLimit)
		: std::vector<qint64>();
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_history_saved_chats());
		if (peers.empty()) {
			box->addRow(object_ptr<Ui::FlatLabel>(
				box,
				tr::lng_serein_history_saved_chats_empty(tr::now),
				st::boxLabel));
		}
		for (const auto id : peers) {
			const auto peer = session->data().peer(
				PeerId(PeerIdHelper(BareId(id))));
			const auto name = peer->name().isEmpty()
				? tr::lng_serein_history_unknown_chat(
					tr::now,
					lt_id,
					QString::number(peerToBareMTPInt(peer->id).v))
				: peer->name();
			const auto open = box->addRow(object_ptr<Ui::LinkButton>(
				box,
				name));
			open->setClickedCallback([=] {
				ShowDeletedMessages(controller, peer);
			});
		}
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

void ConfirmClearHistory(
		gsl::not_null<Window::SessionController*> controller,
		PeerData *peer,
		std::function<void()> done) {
	const auto session = &controller->session();
	const auto peerId = peer ? qint64(peer->id.value) : qint64(0);
	controller->show(Ui::MakeConfirmBox({
		.text = (peer
			? tr::lng_serein_history_clear_chat_sure()
			: tr::lng_serein_history_clear_all_sure()),
		.confirmed = [=](Fn<void()> &&close) {
			const auto cleared = Hooks::ClearHistory(session, peerId);
			close();
			controller->uiShow()->showToast(cleared
				? tr::lng_serein_history_cleared(tr::now)
				: tr::lng_serein_history_clear_failed(tr::now));
			if (cleared && done) {
				done();
			}
		},
		.confirmText = tr::lng_serein_history_clear(),
		.confirmStyle = &st::attentionBoxButton,
	}));
}

} // namespace Serein::HistoryFeature
