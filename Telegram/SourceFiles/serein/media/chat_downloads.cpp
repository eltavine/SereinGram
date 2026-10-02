#include "serein/hooks/media/downloads.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "core/file_utilities.h"
#include "data/data_document.h"
#include "data/data_file_origin.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "main/main_session.h"
#include "serein/core/options.h"
#include "serein/media/download_names.h"
#include "serein/schema/gen/settings/media.h"

namespace Serein {

QDir Hooks::Media::ChatDownloadDirectory(
		not_null<DocumentData*> document,
		const Data::FileOrigin &origin) {
	if (!ForDevice().Get(Serein::Media::kDownloadsPerChat)) {
		return QDir();
	}
	const auto message = std::get_if<FullMsgId>(&origin.data);
	const auto session = &document->session();
	const auto peer = (message && message->peer)
		? session->data().peerLoaded(message->peer)
		: nullptr;
	if (!peer) {
		return QDir();
	}
	const auto custom = Core::App().settings().downloadPath();
	const auto root = custom.isEmpty()
		? File::DefaultDownloadPath(session)
		: (custom == FileDialog::Tmp())
		? QString()
		: custom;
	return root.isEmpty()
		? QDir()
		: QDir(QDir(root).filePath(Serein::Media::DownloadFolderName(
			peer->name(),
			peer->id.value)));
}

} // namespace Serein
