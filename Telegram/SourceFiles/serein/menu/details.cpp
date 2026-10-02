#include "serein/menu/details.h"

#include "serein/hooks/menu/actions.h"
#include "serein/features/stickers/model/owner.h"
#include "serein/display/peer_id.h"
#include "base/unixtime.h"
#include "data/data_document.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/stickers/data_stickers.h"
#include "data/stickers/data_stickers_set.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QLocale>

namespace Serein::Menu {
namespace {

[[nodiscard]] QString FormatDate(TimeId date) {
	return QLocale().toString(
		base::unixtime::parse(date),
		u"yyyy-MM-dd HH:mm:ss"_q);
}

[[nodiscard]] QString StickerSetText(not_null<DocumentData*> document) {
	const auto sticker = document->sticker();
	if (!sticker || !sticker->set) {
		return QString();
	}
	const auto &sets = document->owner().stickers().sets();
	const auto i = sets.find(sticker->set.id);
	const auto known = (i != sets.end());
	const auto shortName = known ? i->second->shortName : sticker->set.shortName;
	const auto title = known ? i->second->title : QString();
	const auto link = shortName.isEmpty()
		? QString()
		: (u"t.me/addstickers/"_q + shortName);
	return title.isEmpty()
		? link
		: link.isEmpty()
		? title
		: (title + u" ("_q + link + u")"_q);
}

[[nodiscard]] QString StickerSetAuthor(not_null<DocumentData*> document) {
	const auto sticker = document->sticker();
	if (!sticker || !sticker->set) {
		return QString();
	}
	const auto ownerId = Stickers::SetOwnerId(sticker->set.id);
	if (!ownerId) {
		return QString();
	}
	const auto id = QString::number(ownerId);
	const auto peer = document->owner().peerLoaded(
		peerFromUser(UserId(ownerId)));
	return peer ? (peer->name() + u" ("_q + id + u")"_q) : id;
}

[[nodiscard]] QString ForwardedName(not_null<HistoryMessageForwarded*> forwarded) {
	return forwarded->originalSender
		? forwarded->originalSender->name()
		: forwarded->originalHiddenSenderInfo
		? forwarded->originalHiddenSenderInfo->name
		: QString();
}

[[nodiscard]] QString Details(not_null<HistoryItem*> item) {
	auto lines = QStringList();
	const auto add = [&](const QString &name, const QString &value) {
		if (!value.isEmpty()) {
			lines.push_back(name + u": "_q + value);
		}
	};
	const auto peer = item->history()->peer;
	add(tr::lng_serein_details_message_id(tr::now),
		QString::number(item->id.bare));
	add(tr::lng_serein_details_chat_id(tr::now),
		Display::PeerIdText(peer, true));
	if (const auto from = item->from(); from != peer) {
		add(tr::lng_serein_details_sender_id(tr::now),
			Display::PeerIdText(from, true));
	}
	add(tr::lng_serein_details_date(tr::now), FormatDate(item->date()));
	if (const auto edited = item->Get<HistoryMessageEdited>()
		; edited && edited->date) {
		add(tr::lng_serein_details_edited(tr::now), FormatDate(edited->date));
	}
	if (const auto forwarded = item->Get<HistoryMessageForwarded>()) {
		add(tr::lng_serein_details_forwarded_from(tr::now),
			ForwardedName(forwarded));
		if (forwarded->originalDate) {
			add(tr::lng_serein_details_forwarded_date(tr::now),
				FormatDate(forwarded->originalDate));
		}
	}
	if (const auto views = item->viewsCount(); views > 0) {
		add(tr::lng_serein_details_views(tr::now), QString::number(views));
	}
	if (const auto media = item->media()) {
		if (const auto document = media->document()) {
			add(tr::lng_serein_details_sticker_set(tr::now),
				StickerSetText(document));
			add(tr::lng_serein_details_sticker_set_author(tr::now),
				StickerSetAuthor(document));
		}
	}
	return lines.join('\n');
}

} // namespace

void InsertDetailsAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !item->isRegular()) {
		return;
	}
	const auto itemId = item->fullId();
	const auto action = Ui::Menu::CreateAction(menu,
		tr::lng_serein_menu_details(tr::now),
		crl::guard(controller, [=] {
			const auto current = controller->session().data().message(itemId);
			if (!current) {
				return;
			}
			const auto text = Details(current);
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				box->setTitle(tr::lng_serein_menu_details());
				const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
					box, text, st::boxLabel));
				label->setSelectable(true);
				box->addButton(tr::lng_close(), [=] { box->closeBox(); });
			}));
		}));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action,
		&st::menuIconInfo, &st::menuIconInfo);
	Tag(menu->insertAction(DeleteActionIndex(menu), std::move(widget)),
		ActionId::MessageDetails);
}

} // namespace Serein::Menu
