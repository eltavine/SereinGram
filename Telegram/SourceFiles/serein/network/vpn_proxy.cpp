#include "serein/network/vpn_proxy.h"

#include "base/timer.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "mtproto/mtproto_proxy_data.h"
#include "serein/core/options.h"
#include "serein/network/vpn_rules.h"
#include "serein/schema/gen/settings/services.h"
#include "window/window_controller.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QUdpSocket>

namespace Serein::Network {
namespace {

constexpr auto kCheckInterval = crl::time(15000);
constexpr auto kProbePort = quint16(443);

[[nodiscard]] InterfaceKind KindOf(QNetworkInterface::InterfaceType type) {
	switch (type) {
	case QNetworkInterface::Loopback: return InterfaceKind::Loopback;
	case QNetworkInterface::Ppp: return InterfaceKind::Ppp;
	case QNetworkInterface::Virtual: return InterfaceKind::Virtual;
	case QNetworkInterface::Unknown: return InterfaceKind::Unknown;
	default: return InterfaceKind::Physical;
	}
}

[[nodiscard]] std::optional<bool> TelegramRouteUsesVpn() {
	auto socket = QUdpSocket();
	socket.setProxy(QNetworkProxy::NoProxy);
	socket.connectToHost(QHostAddress(u"149.154.167.51"_q), kProbePort);
	if (!socket.waitForConnected(0)) {
		return std::nullopt;
	}
	const auto local = socket.localAddress();
	for (const auto &entry : QNetworkInterface::allInterfaces()) {
		for (const auto &address : entry.addressEntries()) {
			if (address.ip() == local) {
				return IsVpnInterface({
					entry.name(),
					entry.humanReadableName(),
					KindOf(entry.type()),
				});
			}
		}
	}
	return std::nullopt;
}

void Notify(const QString &text) {
	if (const auto window = Core::App().activePrimaryWindow()) {
		window->showToast(text);
	}
}

class Controller final : public QObject {
public:
	explicit Controller(QObject *parent);

private:
	void check();
	void resume(bool notify);

	base::Timer _timer;
	bool _started = false;
	rpl::lifetime _lifetime;

};

Controller::Controller(QObject *parent)
: QObject(parent)
, _timer([=] { check(); }) {
	ForDevice().Value(
		ServiceSettings::kPauseProxyOnVpn
	) | rpl::on_next([=](bool enabled) {
		if (!enabled) {
			_timer.cancel();
			resume(false);
			return;
		}
		_timer.callEach(kCheckInterval);
		if (_started) {
			check();
		}
	}, _lifetime);
	_started = true;
}

void Controller::check() {
	const auto vpn = TelegramRouteUsesVpn();
	if (!vpn) {
		return;
	}
	auto &proxy = Core::App().settings().proxy();
	const auto paused = ForDevice().Get(ServiceSettings::kProxyPausedByVpn);
	if (*vpn && !paused && proxy.isEnabled()) {
		Expects(ForDevice().Set(ServiceSettings::kProxyPausedByVpn, true));
		Core::App().setCurrentProxy(
			proxy.selected(),
			MTP::ProxyData::Settings::Disabled);
		Core::App().saveSettingsDelayed();
		Notify(tr::lng_serein_proxy_vpn_paused(tr::now));
	} else if (!*vpn && paused) {
		resume(true);
	}
}

void Controller::resume(bool notify) {
	if (!ForDevice().Get(ServiceSettings::kProxyPausedByVpn)) {
		return;
	}
	Expects(ForDevice().Set(ServiceSettings::kProxyPausedByVpn, false));
	auto &proxy = Core::App().settings().proxy();
	if (!proxy.isDisabled() || !proxy.selected()) {
		return;
	}
	Core::App().setCurrentProxy(
		proxy.selected(),
		MTP::ProxyData::Settings::Enabled);
	Core::App().saveSettingsDelayed();
	if (notify) {
		Notify(tr::lng_serein_proxy_vpn_resumed(tr::now));
	}
}

} // namespace

void StartVpnProxyPause() {
	new Controller(&Core::App());
}

} // namespace Serein::Network
