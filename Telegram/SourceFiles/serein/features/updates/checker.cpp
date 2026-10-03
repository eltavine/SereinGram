#include "serein/features/updates/checker.h"
#include "serein/adapters/qtnetwork/manager.h"

#include "serein/core/build_flags.h"
#include "serein/core/build_info.h"

#include "serein/features/updates/install_target.h"
#include "serein/features/updates/model/manifest.h"
#include "serein/features/updates/model/version.h"
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
constexpr auto kToastDuration = crl::time(12000);
constexpr auto kMaximumResponse = qint64(256 * 1024);
constexpr auto kReleases = "https://github.com/eltavine/SereinGram/releases/";

[[nodiscard]] bool FollowsNightly() {
	return (std::string_view(kBuildChannel) == "nightly")
		&& (std::string_view(kBuildCommit).size() == 40);
}

[[nodiscard]] QString Releases(const QString &path) {
	return QString::fromLatin1(kReleases) + path;
}

[[nodiscard]] std::optional<QString> NewRelease(
		const ReleaseManifest &manifest) {
	if (FollowsNightly()) {
		return (manifest.channel == u"nightly"_q
			&& manifest.commit != QLatin1String(kBuildCommit))
			? std::make_optional(manifest.commit)
			: std::nullopt;
	}
	return (manifest.channel == u"release"_q
		&& IsNewer(manifest.version, QString::fromLatin1(AppVersionStr)))
		? std::make_optional(manifest.tag)
		: std::nullopt;
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
			.url = QUrl(Releases(FollowsNightly()
				? u"download/nightly/release.json"_q
				: u"latest/download/release.json"_q)),
			.maximumSize = kMaximumResponse,
			.timeout = kRequestTimeout,
		}, crl::guard(this, [=](std::optional<QByteArray> body) {
			finished(std::move(body));
		}));
	}

	void finished(std::optional<QByteArray> body) {
		_reply = nullptr;
		const auto manifest = body ? ParseManifest(*body) : std::nullopt;
		const auto id = manifest ? NewRelease(*manifest) : std::nullopt;
		if (!id || *id == _announced) {
			return;
		}
		_announced = *id;
		const auto version = FollowsNightly()
			? u"Nightly (%1)"_q.arg(manifest->commit.left(7))
			: manifest->version;
		const auto page = tr::link(version, Releases(u"tag/"_q + manifest->tag));
		const auto asset = ChooseAsset(*manifest, CurrentInstall());
		const auto window = Core::App().activePrimaryWindow();
		if (!window) {
			return;
		} else if (!asset) {
			window->showToast(tr::lng_serein_update_available(
				tr::now,
				lt_version,
				page,
				tr::marked), kToastDuration);
			return;
		}
		window->showToast(tr::lng_serein_update_download(
			tr::now,
			lt_version,
			page,
			lt_link,
			tr::link(tr::lng_serein_update_download_link(tr::now), asset->url),
			tr::marked), kToastDuration);
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
