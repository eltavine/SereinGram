#include "serein/network/proxy_tools.h"

#include "base/timer.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "main/main_account.h"
#include "main/main_session.h"
#include "main/main_session_show.h"
#include "mtproto/mtproto_proxy_data.h"
#include "mtproto/proxy_check.h"
#include "serein/network/proxy_order.h"
#include "storage/localstorage.h"

namespace Serein::Network {
namespace {

constexpr auto kCheckTimeout = crl::time(15000);

using ProxyData = MTP::ProxyData;
using Connection = MTP::details::AbstractConnection;

auto Busy = false;

struct Entry {
	ProxyData proxy;
	MTP::ProxyCheckConnection v4;
	MTP::ProxyCheckConnection v6;
	ProxyLatency latency;
	bool finished = false;
};

class Check final : public base::has_weak_ptr {
public:
	using Done = Fn<void(std::vector<ProxyLatency>)>;

	Check(
		not_null<MTP::Instance*> mtp,
		const std::vector<ProxyData> &list,
		Done done);
	~Check();

private:
	void start(not_null<MTP::Instance*> mtp, not_null<Entry*> entry);
	void finish(not_null<Entry*> entry, std::optional<int> ping);
	void complete();

	std::vector<std::unique_ptr<Entry>> _entries;
	Done _done;
	base::Timer _timeout;
	bool _completed = false;

};

Check::Check(
	not_null<MTP::Instance*> mtp,
	const std::vector<ProxyData> &list,
	Done done)
: _done(std::move(done))
, _timeout([=] { complete(); }) {
	Busy = true;
	for (const auto &proxy : list) {
		_entries.push_back(std::make_unique<Entry>(Entry{ .proxy = proxy }));
	}
	_timeout.callOnce(kCheckTimeout);
	for (const auto &entry : _entries) {
		start(mtp, entry.get());
	}
}

Check::~Check() {
	Busy = false;
}

void Check::start(not_null<MTP::Instance*> mtp, not_null<Entry*> entry) {
	if (entry->proxy.type == ProxyData::Type::Web) {
		entry->latency.tested = false;
		finish(entry, std::nullopt);
		return;
	}
	MTP::StartProxyCheck(
		mtp,
		entry->proxy,
		Core::App().settings().proxy().tryIPv6(),
		entry->v4,
		entry->v6,
		[=](Connection*, int ping) {
			finish(entry, ping);
		},
		[=](Connection *raw) {
			MTP::DropProxyChecker(entry->v4, entry->v6, raw);
			if (!MTP::HasProxyCheckers(entry->v4, entry->v6)) {
				finish(entry, std::nullopt);
			}
		});
	if (!MTP::HasProxyCheckers(entry->v4, entry->v6)) {
		finish(entry, std::nullopt);
	}
}

void Check::finish(not_null<Entry*> entry, std::optional<int> ping) {
	if (entry->finished) {
		return;
	}
	entry->finished = true;
	entry->latency.ping = ping;
	MTP::ResetProxyCheckers(entry->v4, entry->v6);
	for (const auto &other : _entries) {
		if (!other->finished) {
			return;
		}
	}
	complete();
}

void Check::complete() {
	if (_completed) {
		return;
	}
	_completed = true;
	_timeout.cancel();
	auto result = std::vector<ProxyLatency>();
	for (const auto &entry : _entries) {
		MTP::ResetProxyCheckers(entry->v4, entry->v6);
		result.push_back(entry->latency);
	}
	crl::on_main(this, [=, done = base::take(_done)] {
		done(result);
	});
}

void Apply(
		std::shared_ptr<Main::SessionShow> show,
		const std::vector<ProxyData> &checked,
		std::vector<ProxyLatency> latency,
		bool remove) {
	auto &settings = Core::App().settings().proxy();
	if (settings.list() != checked) {
		show->showToast(tr::lng_serein_proxy_check_changed(tr::now));
		return;
	}
	for (auto i = 0; i != int(checked.size()); ++i) {
		latency[i].selected = (checked[i] == settings.selected());
	}
	if (remove) {
		auto removed = 0;
		for (const auto index : UnavailableProxies(latency)) {
			if (settings.removeFromList(checked[index])) {
				++removed;
			}
		}
		show->showToast(removed
			? tr::lng_serein_proxy_removed(
				tr::now,
				lt_amount,
				QString::number(removed))
			: tr::lng_serein_proxy_none_removed(tr::now));
	} else {
		auto sorted = std::vector<ProxyData>();
		for (const auto index : LatencyOrder(latency)) {
			sorted.push_back(checked[index]);
		}
		settings.list() = std::move(sorted);
		const auto reachable = ranges::count_if(
			latency,
			[](const ProxyLatency &entry) { return entry.ping.has_value(); });
		show->showToast(tr::lng_serein_proxy_sorted(
			tr::now,
			lt_reachable,
			QString::number(reachable),
			lt_total,
			QString::number(checked.size())));
	}
	Local::writeSettings();
}

void Run(std::shared_ptr<Main::SessionShow> show, bool remove) {
	if (Busy) {
		show->showToast(tr::lng_serein_proxy_check_busy(tr::now));
		return;
	}
	const auto list = Core::App().settings().proxy().list();
	if (list.empty()) {
		show->showToast(tr::lng_serein_proxy_check_empty(tr::now));
		return;
	}
	show->showToast(tr::lng_serein_proxy_checking(
		tr::now,
		lt_amount,
		QString::number(list.size())));
	const auto session = &show->session();
	const auto holder = session->lifetime().make_state<
		std::unique_ptr<Check>>();
	*holder = std::make_unique<Check>(
		&session->account().mtp(),
		list,
		[=](std::vector<ProxyLatency> latency) {
			holder->reset();
			Apply(show, list, std::move(latency), remove);
		});
}

} // namespace

void SortProxiesByLatency(std::shared_ptr<Main::SessionShow> show) {
	Run(std::move(show), false);
}

void RemoveUnavailableProxies(std::shared_ptr<Main::SessionShow> show) {
	Run(std::move(show), true);
}

} // namespace Serein::Network
