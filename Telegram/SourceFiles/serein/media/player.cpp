#include "serein/media/player.h"

#include "serein/hooks/media/player.h"
#include "serein/media/options.h"
#include "base/event_filter.h"
#include "core/application.h"
#include "core/file_utilities.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_file_origin.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "media/audio/media_audio.h"
#include "media/player/media_player_instance.h"
#include "media/player/media_player_widget.h"
#include "ui/image/image.h"
#include "ui/abstract_button.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "window/main_window.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include <QtCore/QDir>
#include <QtCore/QSaveFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTimer>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDesktopServices>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTextEdit>

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein::Media {
namespace {

constexpr auto kCoverSize = 240;

struct CoverState {
	std::shared_ptr<Data::DocumentMedia> media;
	QImage image;
	bool canSave = false;
};

void PlayerBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller) {
	box->setTitle(tr::lng_serein_expanded_audio_player());
	box->setWidth(st::boxWideWidth);
	const auto state = box->lifetime().make_state<CoverState>();
	const auto cover = box->addRow(object_ptr<Ui::AbstractButton>(box));
	cover->setAccessibleName(tr::lng_serein_open_cover(tr::now));
	cover->resize(cover->width(), kCoverSize);
	const auto save = box->addRow(object_ptr<Ui::SettingsButton>(
		box, tr::lng_serein_save_cover(), st::settingsButtonNoIcon));
	const auto refresh = [=] {
		const auto track = ::Media::Player::instance()->getState(AudioMsgId::Type::Song);
		const auto document = track.id.audio();
		if (!document || &document->session() != &controller->session()) {
			state->media.reset();
			state->image = QImage();
			state->canSave = false;
		} else {
			state->media = document->createMediaView();
			state->media->thumbnailWanted(track.id.contextId());
			const auto thumbnail = state->media->thumbnail();
			state->image = thumbnail ? thumbnail->original() : QImage();
			const auto item = controller->session().data().message(track.id.contextId());
			state->canSave = !document->forbidsFileSave()
				&& (!item || (!item->forbidsSaving() && !item->isTtlCoveredMedia()));
		}
		const auto available = state->canSave && !state->image.isNull();
		cover->setDisabled(!available);
		save->setDisabled(!available);
		cover->setCursor(available
			? style::cur_pointer : style::cur_default);
		cover->update();
	};
	cover->paintRequest() | rpl::on_next([=] {
		auto painter = QPainter(cover);
		if (!state->image.isNull()) {
			const auto image = state->image.scaled(
				QSize(kCoverSize, kCoverSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			painter.drawImage((cover->width() - image.width()) / 2,
				(kCoverSize - image.height()) / 2, image);
		}
	}, cover->lifetime());
	cover->setClickedCallback([=] {
		if (!state->canSave || state->image.isNull()) {
			return;
		}
		const auto file = std::make_shared<QTemporaryFile>(
			QDir::temp().filePath(u"SereinGram-cover-XXXXXX.png"_q));
		if (file->open() && state->image.save(file.get(), "PNG") && file->flush()) {
			file->close();
			if (QDesktopServices::openUrl(QUrl::fromLocalFile(file->fileName()))) {
				QTimer::singleShot(std::chrono::minutes(5), qApp, [file] {
					file->remove();
				});
				return;
			}
		}
		box->showToast(tr::lng_serein_open_cover_error(tr::now));
	});
	save->setClickedCallback([=] {
		const auto image = state->image;
		if (image.isNull() || !state->canSave) {
			return;
		}
		FileDialog::GetWritePath(Core::App().getFileDialogParent(),
			tr::lng_serein_save_cover(tr::now), u"PNG (*.png)"_q, u"cover.png"_q,
			crl::guard(controller, [=](QString &&path) {
				if (path.isEmpty()) {
					return;
				}
				auto file = QSaveFile(path);
				if (!file.open(QIODevice::WriteOnly)
					|| !image.save(&file, "PNG")
					|| !file.commit()) {
					controller->showToast(tr::lng_serein_snapshot_write_error(tr::now));
				}
			}));
	});
	const auto controls = box->addRow(object_ptr<::Media::Player::Widget>(
		box, box, controller, false));
	controls->setCloseCallback([=] { box->closeBox(); });
	controls->setShowItemCallback([](not_null<const HistoryItem*>) {});
	controller->session().downloaderTaskFinished() | rpl::on_next(refresh, box->lifetime());
	::Media::Player::instance()->trackChanged() | rpl::on_next(refresh, box->lifetime());
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	refresh();
}

[[nodiscard]] bool Editable(QWidget *widget) {
	for (auto current = widget; current; current = current->parentWidget()) {
		if (qobject_cast<QLineEdit*>(current)
			|| qobject_cast<QTextEdit*>(current)
			|| qobject_cast<QPlainTextEdit*>(current)
			|| qobject_cast<QAbstractButton*>(current)
			|| dynamic_cast<Ui::AbstractButton*>(current)
			|| current->testAttribute(Qt::WA_InputMethodEnabled)) {
			return true;
		}
	}
	return false;
}

} // namespace

void ShowExpandedPlayer(not_null<Window::SessionController*> controller) {
	controller->show(Box(PlayerBox, controller));
}

void AttachExpandedPlayer(
		not_null<Ui::RpWidget*> widget,
		not_null<Window::SessionController*> controller) {
	base::install_event_filter(widget, [=](not_null<QEvent*> event) {
		const auto type = event->type();
		if ((type != QEvent::MouseButtonDblClick && type != QEvent::ContextMenu)
			|| !ForDevice().Get(kExpandedAudioPlayer)) {
			return base::EventFilterResult::Continue;
		}
		const auto player = ::Media::Player::instance();
		const auto track = player->getState(AudioMsgId::Type::Song).id.audio();
		if (player->getActiveType() != AudioMsgId::Type::Song
			|| !track
			|| &track->session() != &controller->session()) {
			return base::EventFilterResult::Continue;
		}
		if (type == QEvent::ContextMenu) {
			const auto menu = Ui::CreateChild<Ui::PopupMenu>(widget, st::popupMenuWithIcons);
			menu->addAction(
				tr::lng_serein_expanded_audio_player(tr::now),
				crl::guard(controller, [=] { ShowExpandedPlayer(controller); }));
			menu->popup(static_cast<QContextMenuEvent*>(event.get())->globalPos());
		} else if (static_cast<QMouseEvent*>(event.get())->button() == Qt::LeftButton) {
			ShowExpandedPlayer(controller);
		} else {
			return base::EventFilterResult::Continue;
		}
		return base::EventFilterResult::Cancel;
	});
}

void StartPlaybackTools() {
	base::install_event_filter(&Core::App(), qApp, [](not_null<QEvent*> event) {
		if (event->type() != QEvent::KeyPress || Core::Quitting() || !ForDevice().Get(kSpacePlayback)) {
			return base::EventFilterResult::Continue;
		}
		const auto key = static_cast<QKeyEvent*>(event.get());
		if (key->key() != Qt::Key_Space || key->modifiers() != Qt::NoModifier
			|| key->isAutoRepeat() || Editable(QApplication::focusWidget())) {
			return base::EventFilterResult::Continue;
		}
		const auto window = Core::App().activeWindow();
		if (!window || QApplication::activeWindow() != window->widget().get()) {
			return base::EventFilterResult::Continue;
		}
		const auto controller = window->sessionController();
		if (!controller || Core::App().passcodeLocked()) {
			return base::EventFilterResult::Continue;
		}
		const auto player = ::Media::Player::instance();
		const auto track = player->getState(player->getActiveType()).id.audio();
		if (!track || &track->session() != &controller->session()) {
			return base::EventFilterResult::Continue;
		}
		player->playPause();
		return base::EventFilterResult::Cancel;
	});
}

} // namespace Serein::Media
