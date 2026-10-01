#include "serein/app/updates.h"
#include "serein/adapters/qtnetwork/manager.h"

#include "serein/core/build_flags.h"

#include "serein/features/updates/model/release.h"
#include "serein/hooks/gen/interface.h"
#include "core/application.h"
#include "core/version.h"
#include "lang/lang_keys.h"
#include "window/window_controller.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Serein::App {
namespace {

constexpr auto kFirstCheckDelay = 30 * 1000;
constexpr auto kCheckInterval = 24 * 60 * 60 * 1000;
constexpr auto kRequestTimeout = 30 * 1000;
constexpr auto kMaximumResponse = qint64(1024 * 1024);

class Checker final : public QObject {
public:
	explicit Checker(QObject *parent) : QObject(parent) {
		connect(&_timer, &QTimer::timeout, this, [=] { check(); });
		_timer.start(kFirstCheckDelay);
	}

private:
	void check() {
		_timer.setInterval(kCheckInterval);
		if (_reply || !Hooks::Interface::CheckUpdates()) {
			return;
		}
		auto request = QNetworkRequest(QUrl(
			u"https://api.github.com/repos/eltavine/SereinGram/releases/latest"_q));
		request.setRawHeader("Accept", "application/vnd.github+json");
		request.setRawHeader("User-Agent", "SereinGram");
		request.setTransferTimeout(kRequestTimeout);
		_reply = Adapters::SharedNetwork().get(request);
		connect(_reply, &QNetworkReply::finished, this, [=] { finished(); });
	}

	void finished() {
		const auto reply = _reply.data();
		_reply = nullptr;
		if (!reply) {
			return;
		}
		reply->deleteLater();
		if (reply->error() != QNetworkReply::NoError) {
			return;
		}
		const auto release = Updates::ParseLatestRelease(
			reply->read(kMaximumResponse));
		if (!release
			|| release->tag == _announced
			|| !Updates::IsNewer(
				release->tag,
				QString::fromLatin1(AppVersionStr))) {
			return;
		}
		_announced = release->tag;
		if (const auto window = Core::App().activePrimaryWindow()) {
			window->showToast(tr::lng_serein_update_available(
				tr::now,
				lt_version,
				tr::link(release->tag, release->url),
				tr::marked));
		}
	}

	QTimer _timer;
	QPointer<QNetworkReply> _reply;
	QString _announced;

};

} // namespace

void StartUpdateChecks() {
	if (!kSystemPackage) {
		new Checker(&Core::App());
	}
}

} // namespace Serein::App
