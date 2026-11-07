#include "serein/hooks/media/download_queue.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_download_manager.h"
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
		const auto append = crl::guard(this, [this, ids = std::move(ids)] {
			for (const auto id : ids) {
				if (_queue.size() >= 1000) break;
				const auto item = _session->data().message(id);
				const auto media = item ? item->media() : nullptr;
				if (!media || (!media->document() && !media->photo())
					|| item->forbidsSaving() || item->isTtlCoveredMedia()
					|| ranges::find(_queue, id) != _queue.end()) {
					continue;
				}
				_queue.push_back(id);
			}
			startNext();
		});
		if (!_folder.isEmpty()) {
			append();
			return;
		}
		FileDialog::GetFolder(Core::App().getFileDialogParent(),
			tr::lng_serein_download_queue(tr::now), File::DefaultDownloadPath(_session),
			crl::guard(this, [=, this](QString &&folder) {
				if (!folder.isEmpty()) {
					_folder = folder;
					append();
				}
			}));
	}

	void show(not_null<Window::SessionController*> controller) {
		controller->show(Box(crl::guard(this, [this](not_null<Ui::GenericBox*> box) {
			box->setTitle(tr::lng_serein_download_queue());
			const auto label = box->addRow(object_ptr<Ui::FlatLabel>(box, QString(), st::boxLabel));
			const auto refresh = crl::guard(this, [=, this] {
				auto lines = QStringList();
				lines.push_back(tr::lng_serein_queue_status(tr::now,
					lt_count, QString::number(_queue.size()),
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
			QObject::connect(timer, &QTimer::timeout, box, refresh);
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
	void cancel() {
		_queue.clear();
		if (_ownsTransfer) {
			if (_document && _document->owner()->loading()) _document->owner()->cancel();
			if (_photo && _photo->owner()->loading()) _photo->owner()->cancel();
		}
		_document.reset();
		_photo.reset();
		_ownsTransfer = false;
		_timer.stop();
	}

	void startNext() {
		if (!_session->mtp().networkEnabled()) { _timer.start(); return; }
		while (!_queue.empty() && !_document && !_photo) {
			const auto item = _session->data().message(_queue.front());
			const auto media = item ? item->media() : nullptr;
			if (!media || item->forbidsSaving() || item->isTtlCoveredMedia()) {
				_queue.pop_front();
				++_failed;
				continue;
			}
			const auto document = media->document();
			const auto photo = media->photo();
			if (!document && !photo) {
				_queue.pop_front();
				++_failed;
				continue;
			}
			if ((document && document->loading()) || (photo && photo->loading())) {
				_timer.start();
				return;
			}
			auto name = document ? QFileInfo(document->filename()).fileName()
				: (QString::number(photo->id) + u".jpg"_q);
			if (name.isEmpty() || name == u"."_q || name == u".."_q) name = u"file"_q;
			_path = filedialogNextFilename(name, QString(), _folder);
			_started = Core::App().downloadManager().computeNextStartDate();
			if (document) {
				_document = document->createMediaView();
				document->save(item->fullId(), _path);
			} else {
				_photo = photo->createMediaView();
				_photo->wanted(Data::PhotoSize::Large, item->fullId());
			}
			_ownsTransfer = true;
			_lastProgress = -1.;
			_stalled = 0;
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
		const auto progress = _document ? _document->progress() : _photo->owner()->progress();
		_stalled = (progress > _lastProgress) ? 0 : (_stalled + 1);
		_lastProgress = progress;
		auto done = false;
		auto failed = !item || _stalled > 120;
		if (_document) {
			done = (QFileInfo(_path).size() == _document->owner()->size)
				&& QFileInfo::exists(_path)
				&& !_document->owner()->loading();
			failed |= !done && !_document->owner()->loading();
		} else if (const auto image = _photo->image(Data::PhotoSize::Large)) {
			done = image->original().save(_path, "JPEG");
			failed |= !done;
		} else {
			failed |= !_photo->owner()->loading();
		}
		if (!done && !failed) return;
		if (done && item) {
			Core::App().downloadManager().addLoaded({ item,
				_document ? _document->owner().get() : nullptr,
				_photo ? _photo->owner().get() : nullptr }, _path, _started);
		} else {
			++_failed;
			if (_document && _document->owner()->loading()) _document->owner()->cancel();
			if (_photo && _photo->owner()->loading()) _photo->owner()->cancel();
		}
		_document.reset();
		_photo.reset();
		_ownsTransfer = false;
		if (!_queue.empty()) _queue.pop_front();
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
	double _lastProgress = -1.;
	int _stalled = 0;
	int _failed = 0;
	bool _ownsTransfer = false;

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
