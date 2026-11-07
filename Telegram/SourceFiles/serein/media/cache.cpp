#include "serein/media/cache.h"

#include "serein/hooks/menu/actions.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_media_types.h"
#include "data/data_photo.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "media/player/media_player_instance.h"
#include "media/audio/media_audio.h"
#include "storage/cache/storage_cache_database.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

namespace Serein::Media {
namespace {

constexpr auto kSmallestStreamingPart = int64(128 * 1024);

} // namespace

void ClearDocumentCache(not_null<DocumentData*> document) {
	auto &owner = document->owner();
	if (document->loading()) {
		document->cancel();
	}
	for (const auto &key : {
			document->cacheKey(),
			document->goodThumbnailCacheKey(),
			document->thumbnailLocation().file().cacheKey() }) {
		if (key) {
			owner.cache().remove(key);
		}
	}
	if (const auto base = document->bigFileBaseCacheKey()) {
		const auto parts = document->size / kSmallestStreamingPart + 2;
		for (auto i = int64(0); i != parts; ++i) {
			owner.cacheBigFile().remove({ base.high, base.low + uint64(i) });
		}
	}
}

void InsertClearCacheAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	const auto media = item ? item->media() : nullptr;
	if (!menu || !controller || !media || (!media->document() && !media->photo())) return;
	const auto id = item->fullId();
	Menu::Tag(menu->addAction(tr::lng_serein_menu_clear_cache(tr::now),
		crl::guard(controller, [=] {
			const auto current = controller->session().data().message(id);
			const auto media = current ? current->media() : nullptr;
			if (!media) return;
			if (const auto document = media->document()) {
				const auto player = ::Media::Player::instance();
				for (const auto type : { AudioMsgId::Type::Song, AudioMsgId::Type::Voice }) {
					if (player->getState(type).id.audio() == document) player->stop(type);
				}
				ClearDocumentCache(document);
				if (const auto view = document->activeMediaView()) view->setBytes(QByteArray());
				document->setLoadedInMediaCache(false);
			} else if (const auto photo = media->photo()) {
				photo->cancel();
				photo->clearLocalCache();
			}
			controller->showToast(tr::lng_serein_cache_cleared(tr::now));
		}), &st::menuIconDelete), Menu::ActionId::ClearCache);
}

} // namespace Serein::Media
