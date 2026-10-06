#include "serein/menu/details.h"

#include "serein/features/stickers/model/owner.h"
#include "data/data_document.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/stickers/data_stickers.h"
#include "data/stickers/data_stickers_set.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"

namespace Serein::Menu {
namespace {

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

} // namespace

std::vector<std::pair<QString, QString>> StickerFacts(
		gsl::not_null<HistoryItem*> item) {
	auto result = std::vector<std::pair<QString, QString>>();
	const auto media = item->media();
	const auto document = media ? media->document() : nullptr;
	if (!document) {
		return result;
	}
	const auto add = [&](const QString &label, const QString &value) {
		if (!value.isEmpty()) {
			result.emplace_back(label, value);
		}
	};
	add(tr::lng_serein_details_sticker_set(tr::now), StickerSetText(document));
	add(
		tr::lng_serein_details_sticker_set_author(tr::now),
		StickerSetAuthor(document));
	return result;
}

} // namespace Serein::Menu
