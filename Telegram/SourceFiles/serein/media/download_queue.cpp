#include "serein/hooks/media/download_queue.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_download_manager.h"
#include "data/data_file_origin.h"
#include "data/data_media_types.h"
#include "data/data_photo.h"
#include "data/data_photo_media.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "mtproto/mtp_instance.h"
#include "ui/image/image.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

#include <deque>

#include "styles/style_layers.h"

namespace Serein::Hooks {

class DownloadQueue::Impl final : public QObject {
public:
	explicit Impl(not_null<Main::Session*> session) : _session(session) {
		_timer.setInterval(1000);
		connect(&_timer, &QTimer::timeout, this, [this] { poll(); });
	}

	~Impl() {
		cancel();
	}

	void enqueue(MessageIdsList ids) {
		for (const auto id : ids) {
			if (_queue.size() >= 1000) {
				break;
			}
			const auto item = _session->data().message(id);
			const auto media = item ? item->media() : nullptr;
			if (!media || (!media->document() && !media->photo())
				|| item->forbidsSaving() || item->isTtlCoveredMedia()
				|| ranges::find(_queue, id) != _queue.end()) {
				continue;
			}
			_queue.push_back(id);
		}
		if (_queue.empty() || _choosingFolder || _timer.isActive()) {
			return;
		} else if (!_folder.isEmpty()) {
			startNext();
			return;
		}
		_choosingFolder = true;
		FileDialog::GetFolder(
			Core::App().getFileDialogParent(),
			tr::lng_serein_download_queue(tr::now),
			File::DefaultDownloadPath(_session),
			crl::guard(this, [this](QString &&folder) {
				_choosingFolder = false;
				_folder = std::move(folder);
				startNext();
			}),
			crl::guard(this, [this] {
				_choosingFolder = false;
				cancel();
			}));
	}

	void show(not_null<Window::SessionController*> controller) {
		controller->show(Box(crl::guard(this, [this](not_null<Ui::GenericBox*> box) {
			box->setTitle(tr::lng_serein_download_queue());
			const auto label = box->addRow(object_ptr<Ui::FlatLabel>(box, QString(), st::boxLabel));
			const auto refresh = crl::guard(this, [=, this] {
				auto lines = QStringList();
				lines.push_back(tr::lng_serein_queue_status(tr::now,
					lt_pending, QString::number(_queue.size()),
					lt_amount, QString::number(_failed)));
				if (_document || _photo) {
					const auto progress = _document ? _document->progress() : _photo->progress();
					lines.push_back(QString::number(int(progress * 100.)) + u"%"_q);
				}
				for (const auto id : _queue) {
					const auto item = _session->data().message(id);
					const auto media = item ? item->media() : nullptr;
					if (media) lines.push_back(media->document()
						? media->document()->filename() : tr::lng_serein_downloads_photo(tr::now));
					if (lines.size() > 30) break;
				}
				label->setText(lines.join('\n'));
			});
			const auto timer = new QTimer(box);
			QObject::connect(timer, &QTimer::timeout, box, [=] { refresh(); });
			timer->start(1000);
			refresh();
			box->addButton(tr::lng_serein_queue_cancel(), crl::guard(this, [this, refresh] {
				cancel();
				refresh();
			}));
			box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		})));
	}

private:
	void cancelTransfer() {
		if (_document && _document->owner()->loading()) {
			_document->owner()->cancel();
		}
		if (_photo && _photo->owner()->loading()) {
			_photo->owner()->cancel();
		}
		_document.reset();
		_photo.reset();
	}

	void cancel() {
		_queue.clear();
		cancelTransfer();
		resetProgress();
		_timer.stop();
	}

	void resetProgress() {
		_lastProgress = -1;
		_stalled = 0;
	}

	bool stalled(int64 offset) {
		_stalled = offset > _lastProgress ? 0 : (_stalled + 1);
		_lastProgress = offset;
		return _stalled > 120;
	}

	void skip() {
		_queue.pop_front();
		++_failed;
		resetProgress();
	}

	void startNext() {
		if (_queue.empty()) {
			_timer.stop();
			return;
		} else if (!_session->mtp().networkEnabled()) {
			_timer.start();
			return;
		}
		while (!_queue.empty() && !_document && !_photo) {
			const auto item = _session->data().message(_queue.front());
			const auto media = item ? item->media() : nullptr;
			if (!media || item->forbidsSaving() || item->isTtlCoveredMedia()) {
				skip();
				continue;
			}
			const auto document = media->document();
			const auto photo = media->photo();
			if (!document && !photo) {
				skip();
				continue;
			}
			if ((document && document->loading()) || (photo && photo->loading())) {
				if (stalled(document ? document->loadOffset() : photo->loadOffset())) {
					skip();
					continue;
				}
				_timer.start();
				return;
			}
			_path = document
				? DocumentFileNameForSave(document, false, QString(), QDir(_folder))
				: filedialogNextFilename(QString::number(photo->id) + u".jpg"_q,
					QString(), _folder);
			if (_path.isEmpty()) {
				skip();
				continue;
			}
			_started = Core::App().downloadManager().computeNextStartDate();
			if (document) {
				_document = document->createMediaView();
				document->save(item->fullId(), _path);
			} else {
				_photo = photo->createMediaView();
				_photo->wanted(Data::PhotoSize::Large, item->fullId());
			}
			resetProgress();
			_timer.start();
		}
		if (_queue.empty()) _timer.stop();
	}

	void poll() {
		if (!_session->mtp().networkEnabled()) return;
		if (!_document && !_photo) {
			startNext();
			return;
		}
		const auto item = _queue.empty() ? nullptr : _session->data().message(_queue.front());
		const auto media = item ? item->media() : nullptr;
		const auto document = _document ? _document->owner().get() : nullptr;
		const auto photo = _photo ? _photo->owner().get() : nullptr;
		if (!media || item->forbidsSaving() || item->isTtlCoveredMedia()
			|| (document ? media->document() != document : media->photo() != photo)) {
			cancelTransfer();
			skip();
			startNext();
			return;
		}
		auto done = false;
		auto failed = stalled(document ? document->loadOffset() : photo->loadOffset());
		if (document) {
			const auto info = QFileInfo(_path);
			done = !document->loading() && !document->cancelled()
				&& info.isFile() && info.size() == document->size;
			failed |= !done && !document->loading();
		} else if (const auto image = _photo->image(Data::PhotoSize::Large)) {
			auto file = QFile(_path);
			if (file.open(QIODevice::WriteOnly | QIODevice::NewOnly)) {
				done = image->original().save(&file, "JPEG") && file.flush();
				if (!done) {
					file.remove();
				}
			}
			failed |= !done;
		} else {
			failed |= !photo->loading();
		}
		if (!done && !failed) return;
		if (done) {
			Core::App().downloadManager().addLoaded(
				{ item, document, photo }, _path, _started);
			_document.reset();
			_photo.reset();
			_queue.pop_front();
			resetProgress();
		} else {
			cancelTransfer();
			skip();
		}
		startNext();
	}

	const not_null<Main::Session*> _session;
	std::deque<FullMsgId> _queue;
	std::shared_ptr<Data::DocumentMedia> _document;
	std::shared_ptr<Data::PhotoMedia> _photo;
	QString _folder;
	QString _path;
	QTimer _timer;
	Data::DownloadDate _started = 0;
	int64 _lastProgress = -1;
	int _stalled = 0;
	int _failed = 0;
	bool _choosingFolder = false;

};

DownloadQueue::DownloadQueue(not_null<Main::Session*> session)
: _impl(std::make_unique<Impl>(session)) {
}

DownloadQueue::~DownloadQueue() = default;

void DownloadQueue::enqueue(MessageIdsList ids) {
	_impl->enqueue(std::move(ids));
}

void DownloadQueue::show(not_null<Window::SessionController*> controller) {
	_impl->show(controller);
}

} // namespace Serein::Hooks
