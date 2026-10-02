#include "serein/network/dc_status.h"

#include "base/timer.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "mtproto/connection_abstract.h"
#include "mtproto/facade.h"
#include "mtproto/mtproto_dc_options.h"
#include "mtproto/mtproto_proxy_data.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_widgets.h"

#include <array>

namespace Serein::Network {
namespace {

constexpr auto kDcCount = 5;
constexpr auto kCheckTimeout = crl::time(10000);

using Connection = MTP::details::AbstractConnection;
using Variants = MTP::DcOptions::Variants;
using Report = Fn<void(int index, std::optional<int> ping)>;

class Probe final : public base::has_weak_ptr {
public:
	Probe(
		not_null<MTP::Instance*> mtp,
		const MTP::ProxyData &proxy,
		Report report);

private:
	void start(int index);
	void finish(int index, std::optional<int> ping);

	const not_null<MTP::Instance*> _mtp;
	const MTP::ProxyData _proxy;
	const Report _report;
	std::array<MTP::details::ConnectionPointer, kDcCount> _connections;
	std::array<bool, kDcCount> _finished = {};
	base::Timer _timeout;

};

Probe::Probe(
	not_null<MTP::Instance*> mtp,
	const MTP::ProxyData &proxy,
	Report report)
: _mtp(mtp)
, _proxy(proxy)
, _report(std::move(report))
, _timeout([=] {
	for (auto index = 0; index != kDcCount; ++index) {
		finish(index, std::nullopt);
	}
}) {
	_timeout.callOnce(kCheckTimeout);
	for (auto index = 0; index != kDcCount; ++index) {
		start(index);
	}
}

void Probe::start(int index) {
	const auto dcId = MTP::DcId(index + 1);
	const auto protocol = (_proxy.type == MTP::ProxyData::Type::Http)
		? Variants::Http
		: Variants::Tcp;
	const auto connect = [&](
			const QString &host,
			int port,
			const bytes::vector &secret) {
		auto &connection = _connections[index];
		connection = Connection::Create(
			_mtp,
			protocol,
			QThread::currentThread(),
			secret,
			_proxy);
		const auto raw = connection.get();
		const auto weak = base::make_weak(this);
		raw->connect(raw, &Connection::connected, [=] {
			if (const auto strong = weak.get()) {
				strong->finish(index, int(raw->pingTime()));
			}
		});
		const auto failed = [=] {
			if (const auto strong = weak.get()) {
				strong->finish(index, std::nullopt);
			}
		};
		raw->connect(raw, &Connection::disconnected, failed);
		raw->connect(raw, &Connection::error, failed);
		raw->connectToServer(host, port, secret, dcId, false);
	};
	if (_proxy.type == MTP::ProxyData::Type::Mtproto) {
		connect(_proxy.host, _proxy.port, _proxy.secretFromMtprotoPassword());
		return;
	}
	const auto options = _mtp->dcOptions().lookup(
		dcId,
		MTP::DcType::Regular,
		true);
	const auto &list = options.data[Variants::IPv4][protocol];
	if (list.empty()) {
		finish(index, std::nullopt);
		return;
	}
	const auto &endpoint = list.front();
	connect(
		QString::fromStdString(endpoint.ip),
		endpoint.port,
		endpoint.secret);
}

void Probe::finish(int index, std::optional<int> ping) {
	if (_finished[index]) {
		return;
	}
	_finished[index] = true;
	_connections[index] = nullptr;
	_report(index, ping);
}

[[nodiscard]] QString City(int index) {
	switch (index) {
	case 0:
	case 2: return tr::lng_serein_dc_miami(tr::now);
	case 1:
	case 3: return tr::lng_serein_dc_amsterdam(tr::now);
	}
	return tr::lng_serein_dc_singapore(tr::now);
}

[[nodiscard]] QString StatusText(std::optional<int> ping) {
	return ping
		? tr::lng_proxy_available(tr::now, lt_ping, QString::number(*ping))
		: tr::lng_proxy_unavailable(tr::now);
}

[[nodiscard]] MTP::ProxyData MeasuredProxy() {
	const auto &settings = Core::App().settings().proxy();
	const auto selected = settings.selected();
	return (settings.isEnabled()
		&& selected
		&& selected.type != MTP::ProxyData::Type::Web)
		? selected
		: MTP::ProxyData();
}

void DatacenterBox(
		not_null<Ui::GenericBox*> box,
		not_null<MTP::Instance*> mtp) {
	struct State {
		std::array<rpl::variable<QString>, kDcCount> statuses;
		std::unique_ptr<Probe> probe;
	};
	const auto state = box->lifetime().make_state<State>();
	const auto proxy = MeasuredProxy();
	box->setTitle(tr::lng_serein_dc_status());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		proxy
			? tr::lng_serein_dc_via_proxy(
				tr::now,
				lt_proxy,
				proxy.host + u':' + QString::number(proxy.port))
			: tr::lng_serein_dc_via_direct(tr::now),
		st::boxDividerLabel));
	const auto main = mtp->mainDcId();
	for (auto index = 0; index != kDcCount; ++index) {
		auto name = u"DC%1 · %2"_q.arg(index + 1).arg(City(index));
		if (main == index + 1) {
			name += u" · "_q + tr::lng_serein_dc_this_account(tr::now);
		}
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			state->statuses[index].value(
			) | rpl::map([=](const QString &status) {
				return name + u'\n' + status;
			}),
			st::boxLabel));
	}
	const auto check = [=] {
		for (auto &status : state->statuses) {
			status = tr::lng_proxy_checking(tr::now);
		}
		state->probe = std::make_unique<Probe>(mtp, proxy, crl::guard(box, [=](
				int index,
				std::optional<int> ping) {
			state->statuses[index] = StatusText(ping);
		}));
	};
	check();
	box->addButton(tr::lng_serein_dc_recheck(), check);
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void ShowDatacenterStatus(
		gsl::not_null<Window::SessionController*> controller) {
	const auto mtp = &controller->session().mtp();
	controller->show(Box(DatacenterBox, mtp));
}

} // namespace Serein::Network
