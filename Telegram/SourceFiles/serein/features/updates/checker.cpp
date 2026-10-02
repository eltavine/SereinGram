#include "serein/features/updates/checker.h"
#include "serein/adapters/qtnetwork/manager.h"

#include "serein/core/build_flags.h"
#include "serein/core/build_info.h"

#include "serein/features/updates/model/release.h"
#include "serein/hooks/gen/interface.h"
#include "core/application.h"
#include "core/version.h"
#include "lang/lang_keys.h"
#include "window/window_controller.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkReply>

#include <string_view>

namespace Serein::Updates {
namespace {

constexpr auto kFirstCheckDelay = 30 * 1000;
constexpr auto kCheckInterval = 24 * 60 * 60 * 1000;
constexpr auto kRequestTimeout = 30 * 1000;
constexpr auto kMaximumResponse = qint64(1024 * 1024);
constexpr auto kLatestUrl = "https://api.github.com/repos/eltavine/SereinGram/releases/latest";
constexpr auto kNightlyUrl = "https://api.github.com/repos/eltavine/SereinGram/releases/tags/nightly";

[[nodiscard]] bool FollowsNightly() {
	return (std::string_view(kBuildChannel) == "nightly")
		&& (std::string_view(kBuildCommit).size() == 40);
}

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
			.url = QUrl(QString::fromLatin1(
				FollowsNightly() ? kNightlyUrl : kLatestUrl)),
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
		} else if (FollowsNightly()) {
			const auto nightly = Updates::ParseNightlyRelease(*body);
			if (nightly && nightly->commit != QLatin1String(kBuildCommit)) {
				announce(
					nightly->commit,
					u"Nightly (%1)"_q.arg(nightly->commit.left(7)),
					nightly->url);
			}
			return;
		}
		const auto release = Updates::ParseLatestRelease(*body);
		if (release
			&& Updates::IsNewer(
				release->tag,
				QString::fromLatin1(AppVersionStr))) {
			announce(release->tag, release->tag, release->url);
		}
	}

	void announce(
			const QString &id,
			const QString &version,
			const QString &url) {
		if (id == _announced) {
			return;
		}
		_announced = id;
		if (const auto window = Core::App().activePrimaryWindow()) {
			window->showToast(tr::lng_serein_update_available(
				tr::now,
				lt_version,
				tr::link(version, url),
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
