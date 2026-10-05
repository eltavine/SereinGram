#include "serein/features/history/menu.h"

#include "serein/features/history/backend.h"
#include "serein/features/history/capture.h"
#include "serein/features/history/copies.h"
#include "serein/features/history/model/recorder.h"
#include "serein/features/history/viewer.h"
#include "serein/hooks/menu/actions.h"
#include "core/file_utilities.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace Serein::HistoryFeature {
namespace {

void InsertAction(
		Ui::PopupMenu *menu,
		const QString &text,
		Fn<void()> callback,
		const style::icon *icon,
		Menu::ActionId id) {
	const auto action = Ui::Menu::CreateAction(menu, text, std::move(callback));
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(),
		menu->menu()->st(),
		action,
		icon,
		icon);
	Menu::Tag(
		menu->insertAction(Menu::DeleteActionIndex(menu), std::move(widget)),
		id);
}

[[nodiscard]] bool OpenCachedMedia(
		gsl::not_null<Main::Session*> session,
		const History::Record &record) {
	const auto name = SafeFileName(record.cachedMediaName);
	if (name.isEmpty()) {
		return false;
	}
	const auto bytes = CachedMediaBytes(session, record);
	const auto folder = QDir::temp().filePath(u"SereinGram"_q);
	const auto path = QDir(folder).filePath(name);
	auto file = QFile(path);
	if (!bytes
		|| !QDir().mkpath(folder)
		|| !file.open(QIODevice::WriteOnly)
		|| file.write(*bytes) != bytes->size()) {
		return false;
	}
	file.close();
	File::Launch(path);
	return true;
}

} // namespace

void InsertEditHistoryAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller) {
		return;
	}
	const auto copy = FindCopy(item);
	if (copy && copy->version) {
		return;
	} else if (!copy && !IsServerMsgId(item->id)) {
		return;
	}
	const auto peer = item->history()->peer;
	const auto messageId = copy ? copy->record.messageId : item->id.bare;
	if (!HasEditHistory(&controller->session(), peer, messageId)) {
		return;
	}
	InsertAction(
		menu,
		tr::lng_serein_menu_edit_history(tr::now),
		crl::guard(controller, [=] {
			ShowEditHistory(controller, peer, messageId);
		}),
		&st::menuIconEdit,
		Menu::ActionId::EditHistory);
}

void InsertDeletedMessagesAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || FindCopy(item)) {
		return;
	}
	const auto peer = item->history()->peer;
	if (!HasDeletedMessages(&controller->session(), peer)) {
		return;
	}
	InsertAction(
		menu,
		tr::lng_serein_menu_deleted_messages(tr::now),
		crl::guard(controller, [=] {
			ShowDeletedMessages(controller, peer);
		}),
		&st::menuIconRestore,
		Menu::ActionId::DeletedMessages);
}

void InsertHistoryExclusionAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || FindCopy(item)) {
		return;
	}
	const auto session = &controller->session();
	const auto policy = HistoryFeature::Read(ForAccount(session));
	if (!policy.saveDeleted && !policy.saveEdits) {
		return;
	}
	const auto peerId = qint64(item->history()->peer->id.value);
	const auto excluded = policy.excludedPeers.contains(peerId);
	InsertAction(
		menu,
		(excluded
			? tr::lng_serein_menu_history_include
			: tr::lng_serein_menu_history_exclude)(tr::now),
		crl::guard(controller, [=] {
			Expects(HistoryFeature::SetExcluded(
				ForAccount(session),
				peerId,
				!excluded));
		}),
		&st::menuIconArchive,
		Menu::ActionId::HistoryExclusion);
}

void InsertRestoredMediaAction(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	const auto copy = (menu && item && controller) ? FindCopy(item) : nullptr;
	if (!copy) {
		return;
	}
	const auto record = copy->record;
	const auto path = record.localPath;
	if (!path.isEmpty() && QFileInfo::exists(path)) {
		InsertAction(
			menu,
			tr::lng_serein_history_open_file(tr::now),
			[=] { File::Launch(path); },
			&st::menuIconShowInFolder,
			Menu::ActionId::RestoredMedia);
	} else if (!record.cachedMediaName.isEmpty()) {
		const auto session = &controller->session();
		InsertAction(
			menu,
			tr::lng_serein_history_open_media(tr::now),
			crl::guard(controller, [=] {
				if (!OpenCachedMedia(session, record)) {
					controller->showToast(
						tr::lng_serein_history_media_missing(tr::now));
				}
			}),
			&st::menuIconPhoto,
			Menu::ActionId::RestoredMedia);
	}
}

} // namespace Serein::HistoryFeature
