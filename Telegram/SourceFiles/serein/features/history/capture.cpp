#include "serein/features/history/capture.h"

#include "serein/features/history/entities.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_photo.h"
#include "data/data_photo_media.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "ui/text/text_entity.h"

#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

namespace Serein::HistoryFeature {

QString SafeFileName(const QString &name) {
	auto result = QFileInfo(name).fileName().left(255);
	for (auto &ch : result) {
		if (ch < QChar(0x20) || QStringView(u"<>:\"/\\|?*").contains(ch)) {
			ch = u'_';
		}
	}
	return result;
}

Snapshot TakeSnapshot(gsl::not_null<HistoryItem*> item) {
	auto result = Snapshot();
	result.peerId = qint64(item->history()->peer->id.value);
	result.messageId = item->id.bare;
	result.topicRootId = item->topicRootId().bare;
	result.date = item->date();
	const auto from = item->from();
	result.fromPeerId = qint64(from->id.value);
	const auto user = from->asUser();
	result.fromBot = user && user->isBot();
	const auto &text = item->originalText();
	result.text = text.text;
	for (const auto &entity : text.entities) {
		const auto name = EntityName(entity.type());
		if (!name.isEmpty() && entity.offset() >= 0 && entity.length() > 0) {
			result.entities.push_back({
				name,
				entity.offset(),
				entity.length(),
				entity.data(),
			});
		}
	}
	if (const auto media = item->media()) {
		result.mediaSummary = media->notificationText().text;
		if (const auto document = media->document()) {
			result.localPath = document->filepath(true);
		}
	}
	return result;
}

std::optional<CachedMedia> CaptureCachedMedia(
		gsl::not_null<HistoryItem*> item) {
	const auto media = item->media();
	if (const auto photo = media ? media->photo() : nullptr) {
		const auto view = photo->activeMediaView();
		auto bytes = view
			? view->imageBytes(Data::PhotoSize::Large)
			: QByteArray();
		if (!bytes.isEmpty()) {
			return CachedMedia{
				std::move(bytes),
				u"photo_%1.jpg"_q.arg(item->id.bare),
			};
		}
	} else if (const auto document = media ? media->document() : nullptr) {
		const auto view = document->activeMediaView();
		auto bytes = view ? view->bytes() : QByteArray();
		if (!bytes.isEmpty()) {
			auto name = SafeFileName(document->filename());
			if (name.isEmpty()) {
				const auto suffix = QMimeDatabase().mimeTypeForName(
					document->mimeString()).preferredSuffix();
				name = u"file_%1"_q.arg(item->id.bare)
					+ (suffix.isEmpty() ? QString() : (u'.' + suffix));
			}
			return CachedMedia{ std::move(bytes), name };
		}
	}
	return std::nullopt;
}

} // namespace Serein::HistoryFeature
