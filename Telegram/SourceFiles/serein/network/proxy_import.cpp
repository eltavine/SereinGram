#include "serein/network/proxy_import.h"
#include "serein/adapters/qtnetwork/manager.h"

#include "serein/hooks/services/model.h"
#include "serein/network/proxy_subscription.h"
#include "boxes/connection_box.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "storage/localstorage.h"
#include "ui/layers/show.h"

namespace Serein::Network {
namespace {

constexpr auto kRequestTimeout = 30 * 1000;

void Import(const std::shared_ptr<Ui::Show> &show, const QByteArray &body) {
	auto &proxies = Core::App().settings().proxy();
	auto added = 0;
	auto present = 0;
	for (const auto &link : ExtractProxyLinks(QString::fromUtf8(body))) {
		const auto proxy = ProxiesBoxController::ProxyFromLink(link);
		if (!proxy) {
			continue;
		} else if (proxies.indexInList(proxy) >= 0) {
			++present;
		} else {
			proxies.addToList(proxy);
			++added;
		}
	}
	if (added) {
		Local::writeSettings();
	}
	show->showToast((added || present)
		? tr::lng_serein_proxy_subscription_done(
			tr::now,
			lt_added,
			QString::number(added),
			lt_existing,
			QString::number(present))
		: tr::lng_serein_proxy_subscription_empty(tr::now));
}

} // namespace

void UpdateProxySubscription(std::shared_ptr<Ui::Show> show) {
	const auto url = QUrl(
		ForDevice().Get(ServiceSettings::kProxySubscription),
		QUrl::StrictMode);
	if (!url.isValid() || url.scheme() != u"https"_q) {
		show->showToast(tr::lng_serein_proxy_subscription_invalid(tr::now));
		return;
	}
	Adapters::Download({
		.url = url,
		.maximumSize = kSubscriptionMaximumSize,
		.timeout = kRequestTimeout,
	}, [=](std::optional<QByteArray> body) {
		if (!body) {
			show->showToast(tr::lng_serein_proxy_subscription_failed(
				tr::now));
			return;
		}
		Import(show, *body);
	});
}

} // namespace Serein::Network
