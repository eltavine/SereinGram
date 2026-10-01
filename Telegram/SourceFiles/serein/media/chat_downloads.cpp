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
#include "serein/schema/gen/settings/media.h"

namespace Serein {
namespace {

constexpr auto kMaxFolderName = 64;

[[nodiscard]] bool Reserved(const QString &name) {
	static const auto names = QStringList{
		u"CON"_q, u"PRN"_q, u"AUX"_q, u"NUL"_q,
		u"COM1"_q, u"COM2"_q, u"COM3"_q, u"COM4"_q, u"COM5"_q,
		u"COM6"_q, u"COM7"_q, u"COM8"_q, u"COM9"_q,
		u"LPT1"_q, u"LPT2"_q, u"LPT3"_q, u"LPT4"_q, u"LPT5"_q,
		u"LPT6"_q, u"LPT7"_q, u"LPT8"_q, u"LPT9"_q,
	};
	return names.contains(name.section(u'.', 0, 0), Qt::CaseInsensitive);
}

[[nodiscard]] QString FolderName(not_null<PeerData*> peer) {
	static const auto forbidden = u"<>:\"/\\|?*"_q;
	auto result = QString();
	for (const auto ch : peer->name()) {
		result += (ch.unicode() < 32 || forbidden.contains(ch)) ? u'_' : ch;
	}
	result = result.left(kMaxFolderName).trimmed();
	while (result.endsWith(u'.')) {
		result.chop(1);
	}
	if (result.isEmpty()) {
		return QString::number(peer->id.value);
	}
	return Reserved(result) ? (result + u'_') : result;
}

} // namespace

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
		: QDir(QDir(root).filePath(FolderName(peer)));
}

} // namespace Serein
