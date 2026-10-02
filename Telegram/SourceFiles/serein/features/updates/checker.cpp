#include "serein/features/updates/checker.h"
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
#include <QtNetwork/QNetworkReply>

namespace Serein::Updates {
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
		_reply = Adapters::Download({
			.url = QUrl(u"https://api.github.com/repos/eltavine/SereinGram/releases/latest"_q),
			.maximumSize = kMaximumResponse,
			.timeout = kRequestTimeout,
			.headers = { { "Accept", "application/vnd.github+json" } },
		}, crl::guard(this, [=](std::optional<QByteArray> body) {
			finished(std::move(body));
		}));
	}

	void finished(std::optional<QByteArray> body) {
		_reply = nullptr;
		if (!body) {
			return;
		}
		const auto release = Updates::ParseLatestRelease(*body);
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

} // namespace Serein::Updates
