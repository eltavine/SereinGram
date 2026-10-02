#include "serein/chats/chat_cache.h"

#include "base/flat_set.h"
#include "data/data_document.h"
#include "data/data_media_types.h"
#include "data/data_photo.h"
#include "data/data_session.h"
#include "data/data_web_page.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "storage/cache/storage_cache_database.h"

namespace Serein::Chats {
namespace {

constexpr auto kSmallestStreamingPart = int64(128 * 1024);

void Collect(
		not_null<History*> history,
		base::flat_set<not_null<DocumentData*>> &documents,
		base::flat_set<not_null<PhotoData*>> &photos) {
	const auto add = [&](DocumentData *document, PhotoData *photo) {
		if (document && !document->sticker()) {
			documents.emplace(document);
		} else if (photo) {
			photos.emplace(photo);
		}
	};
	for (const auto &block : history->blocks) {
		for (const auto &view : block->messages) {
			const auto media = view->data()->media();
			if (!media) {
				continue;
			}
			add(media->document(), media->photo());
			if (const auto page = media->webpage()) {
				add(page->document, page->photo);
			}
		}
	}
}

void ClearDocument(not_null<DocumentData*> document) {
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

} // namespace

int ClearLoadedMediaCache(not_null<History*> history) {
	auto documents = base::flat_set<not_null<DocumentData*>>();
	auto photos = base::flat_set<not_null<PhotoData*>>();
	Collect(history, documents, photos);
	if (const auto migrated = history->migrateFrom()) {
		Collect(migrated, documents, photos);
	}
	for (const auto &document : documents) {
		ClearDocument(document);
	}
	for (const auto &photo : photos) {
		photo->clearLocalCache();
	}
	return int(documents.size() + photos.size());
}

} // namespace Serein::Chats
