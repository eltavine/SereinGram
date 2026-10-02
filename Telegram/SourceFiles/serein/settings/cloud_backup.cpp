#include "serein/settings/cloud_backup.h"

#include "api/api_common.h"
#include "apiwrap.h"
#include "base/unixtime.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_file_origin.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_types.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "storage/localimageloader.h"
#include "storage/storage_media_prepare.h"
#include "ui/chat/attach/attach_prepare.h"
#include "window/window_session_controller.h"
#include "styles/style_boxes.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

namespace Serein {
namespace {

constexpr auto kSearchLimit = 20;
constexpr auto kMaximumBackupBytes = 8 * 1024 * 1024;
constexpr auto kFileName = "sereingram-settings.json";
constexpr auto kTag = "#SereinGramSettings";

struct Loading {
	std::shared_ptr<Data::DocumentMedia> media;
	rpl::lifetime lifetime;
};

[[nodiscard]] QString WriteBackupFile(const QByteArray &data) {
	const auto folder = QDir::temp().filePath(
		u"SereinGram/backup-%1"_q.arg(base::unixtime::now()));
	const auto path = QDir(folder).filePath(QString::fromLatin1(kFileName));
	auto file = QFile(path);
	if (!QDir().mkpath(folder)
		|| !file.open(QIODevice::WriteOnly)
		|| file.write(data) != data.size()) {
		return QString();
	}
	return path;
}

[[nodiscard]] DocumentData *FindBackup(
		not_null<Main::Session*> session,
		const MTPmessages_Messages &result,
		FullMsgId *itemId) {
	auto &owner = session->data();
	const auto messages = result.match([](
			const MTPDmessages_messagesNotModified &) {
		return QVector<MTPMessage>();
	}, [&](const auto &data) {
		owner.processUsers(data.vusers());
		owner.processChats(data.vchats());
		return data.vmessages().v;
	});
	for (const auto &message : messages) {
		if (!DateFromMessage(message)) {
			continue;
		}
		const auto item = owner.addNewMessage(
			message,
			MessageFlags(),
			NewMessageType::Existing);
		const auto media = item ? item->media() : nullptr;
		const auto document = media ? media->document() : nullptr;
		if (document
			&& document->filename() == QString::fromLatin1(kFileName)
			&& document->size <= kMaximumBackupBytes) {
			*itemId = item->fullId();
			return document;
		}
	}
	return nullptr;
}

[[nodiscard]] QByteArray LoadedBytes(
		not_null<DocumentData*> document,
		const std::shared_ptr<Data::DocumentMedia> &media) {
	if (auto bytes = media->bytes(); !bytes.isEmpty()) {
		return bytes;
	}
	auto file = QFile(document->filepath(true));
	return file.open(QIODevice::ReadOnly)
		? file.read(kMaximumBackupBytes)
		: QByteArray();
}

void Download(
		not_null<Window::SessionController*> controller,
		not_null<DocumentData*> document,
		FullMsgId itemId,
		Fn<void(QByteArray)> done) {
	const auto loading = controller->lifetime().make_state<Loading>();
	loading->media = document->createMediaView();
	document->save(itemId, QString());
	const auto finish = crl::guard(controller, [=] {
		auto bytes = LoadedBytes(document, loading->media);
		if (bytes.isEmpty()) {
			controller->showToast(
				tr::lng_serein_config_restore_failed(tr::now));
		} else {
			done(std::move(bytes));
		}
	});
	if (loading->media->loaded()) {
		finish();
		return;
	}
	controller->session().downloaderTaskFinished(
	) | rpl::filter([=] {
		return loading->media->loaded() || !document->loading();
	}) | rpl::take(1) | rpl::on_next([=] {
		finish();
	}, loading->lifetime);
}

} // namespace

void BackUpToSavedMessages(
		gsl::not_null<Window::SessionController*> controller,
		const QByteArray &data) {
	const auto session = &controller->session();
	const auto path = WriteBackupFile(data);
	auto list = path.isEmpty()
		? Ui::PreparedList()
		: Storage::PrepareMediaList(
			QStringList{ path },
			st::sendMediaPreviewSize,
			session->premium());
	if (path.isEmpty()
		|| list.error != Ui::PreparedList::Error::None
		|| list.files.size() != 1) {
		controller->showToast(tr::lng_serein_config_backup_failed(tr::now));
		return;
	}
	list.files.front().caption = { QString::fromLatin1(kTag), {} };
	auto action = Api::SendAction(session->data().history(session->user()));
	action.clearDraft = false;
	session->api().sendFiles(
		std::move(list),
		SendMediaType::File,
		nullptr,
		action);
	controller->showToast(tr::lng_serein_config_backup_done(tr::now));
}

void RestoreFromSavedMessages(
		gsl::not_null<Window::SessionController*> controller,
		Fn<void(QByteArray)> done) {
	const auto session = &controller->session();
	session->api().request(MTPmessages_Search(
		MTP_flags(0),
		MTP_inputPeerSelf(),
		MTP_string(QString::fromLatin1(kTag)),
		MTP_inputPeerEmpty(), // from_id
		MTP_inputPeerEmpty(), // saved_peer_id
		MTPVector<MTPReaction>(), // saved_reaction
		MTP_int(0), // top_msg_id
		MTP_inputMessagesFilterDocument(),
		MTP_int(0), // min_date
		MTP_int(0), // max_date
		MTP_int(0), // offset_id
		MTP_int(0), // add_offset
		MTP_int(kSearchLimit),
		MTP_int(0), // max_id
		MTP_int(0), // min_id
		MTP_long(0) // hash
	)).done(crl::guard(controller, [=](const MTPmessages_Messages &result) {
		auto itemId = FullMsgId();
		if (const auto document = FindBackup(session, result, &itemId)) {
			Download(controller, document, itemId, done);
		} else {
			controller->showToast(
				tr::lng_serein_config_restore_none(tr::now));
		}
	})).fail(crl::guard(controller, [=](const MTP::Error &) {
		controller->showToast(tr::lng_serein_config_restore_failed(tr::now));
	})).send();
}

} // namespace Serein
