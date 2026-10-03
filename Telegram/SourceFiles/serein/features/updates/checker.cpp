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

[[nodiscard]] TextWithEntities AvailableText(const AvailableUpdate &update) {
	const auto page = tr::link(update.version, update.page);
	if (update.download.isEmpty()) {
		return tr::lng_serein_update_available(
			tr::now,
			lt_version,
			page,
			tr::marked);
	}
	return tr::lng_serein_update_download(
		tr::now,
		lt_version,
		page,
		lt_link,
		tr::link(tr::lng_serein_update_download_link(tr::now), update.download),
		tr::marked);
}

[[nodiscard]] std::optional<AvailableUpdate> NewRelease(
		const ReleaseManifest &manifest) {
	const auto newer = FollowsNightly()
		? (manifest.channel == u"nightly"_q
			&& manifest.commit != QLatin1String(kBuildCommit))
		: (manifest.channel == u"release"_q
			&& IsNewer(manifest.version, QString::fromLatin1(AppVersionStr)));
	if (!newer) {
		return std::nullopt;
	}
	const auto asset = ChooseAsset(manifest, CurrentInstall());
	return AvailableUpdate{
		.id = FollowsNightly() ? manifest.commit : manifest.tag,
		.version = FollowsNightly()
			? u"Nightly (%1)"_q.arg(manifest.commit.left(7))
			: manifest.version,
		.page = Releases(u"tag/"_q + manifest.tag),
		.download = asset ? asset->url : QString(),
	};
}

class Checker final : public QObject {
public:
	explicit Checker(QObject *parent) : QObject(parent) {
		connect(&_timer, &QTimer::timeout, this, [=] {
			_timer.setInterval(kCheckInterval);
			if (Hooks::Interface::CheckUpdates()) {
				request(nullptr);
			}
		});
		_timer.start(kFirstCheckDelay);
	}

	void request(Fn<void(CheckResult)> done) {
		if (done) {
			_waiting.push_back(std::move(done));
		} else {
			_announce = true;
		}
		if (_reply) {
			return;
		}
		_checking = true;
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

	[[nodiscard]] rpl::producer<bool> checking() const {
		return _checking.value();
	}
	[[nodiscard]] auto available() const
	-> rpl::producer<std::optional<AvailableUpdate>> {
		return _available.value();
	}

private:
	void finished(std::optional<QByteArray> body) {
		_reply = nullptr;
		_checking = false;
		const auto manifest = body ? ParseManifest(*body) : std::nullopt;
		const auto update = manifest ? NewRelease(*manifest) : std::nullopt;
		const auto result = !manifest
			? CheckResult::Failed
			: update
			? CheckResult::Available
			: CheckResult::UpToDate;
		if (manifest) {
			_available = update;
		}
		const auto manual = !_waiting.empty();
		const auto announce = base::take(_announce);
		if (update && (manual || (announce && update->id != _announced))) {
			_announced = update->id;
			if (const auto window = Core::App().activePrimaryWindow()) {
				window->showToast(AvailableText(*update), kToastDuration);
			}
		}
		for (const auto &done : base::take(_waiting)) {
			done(result);
		}
	}

	QTimer _timer;
	QPointer<QNetworkReply> _reply;
	std::vector<Fn<void(CheckResult)>> _waiting;
	rpl::variable<bool> _checking = false;
	rpl::variable<std::optional<AvailableUpdate>> _available;
	QString _announced;
	bool _announce = false;

};

QPointer<Checker> Instance;

} // namespace

void StartUpdateChecks() {
	if (!kSystemPackage && !Instance) {
		Instance = new Checker(&Core::App());
	}
}

bool UpdateChecksAvailable() {
	return Instance != nullptr;
}

QString ReleasesUrl() {
	return Releases(QString());
}

void CheckForUpdatesNow(Fn<void(CheckResult)> done) {
	if (Instance) {
		Instance->request(std::move(done));
	} else if (done) {
		done(CheckResult::Failed);
	}
}

rpl::producer<bool> CheckingValue() {
	if (!Instance) {
		return rpl::single(false);
	}
	return Instance->checking();
}

rpl::producer<std::optional<AvailableUpdate>> AvailableValue() {
	if (!Instance) {
		return rpl::single(std::optional<AvailableUpdate>());
	}
	return Instance->available();
}

} // namespace Serein::Updates
