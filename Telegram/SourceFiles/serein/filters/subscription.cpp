#include "serein/filters/subscription.h"

#include "serein/filters/model.h"
#include "core/application.h"
#include "lang/lang_keys.h"
#include "ui/layers/show.h"

#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Serein::Filters {
namespace {

constexpr auto kRequestTimeout = 30 * 1000;
constexpr auto kMaximumSize = qint64(1024 * 1024);
constexpr auto kFirstUpdateDelay = 60 * 1000;
constexpr auto kUpdateInterval = 24 * 60 * 60 * 1000;

void Notify(const std::shared_ptr<Ui::Show> &show, const QString &text) {
	if (show) {
		show->showToast(text);
	}
}

void Store(const std::shared_ptr<Ui::Show> &show, const QByteArray &body) {
	const auto rules = ReadRuleList(body);
	if (!rules || !ForDevice().Set(kSubscribedRules, body)) {
		Notify(show, tr::lng_serein_filter_subscription_invalid(tr::now));
		return;
	}
	Notify(show, tr::lng_serein_filter_subscription_done(
		tr::now,
		lt_amount,
		QString::number(rules->size())));
}

} // namespace

void UpdateRuleSubscription(std::shared_ptr<Ui::Show> show) {
	const auto url = QUrl(
		ForDevice().Get(kRuleSubscription),
		QUrl::StrictMode);
	if (!url.isValid() || url.scheme() != u"https"_q) {
		Notify(show, tr::lng_serein_filter_subscription_missing(tr::now));
		return;
	}
	const auto network = new QNetworkAccessManager(&Core::App());
	auto request = QNetworkRequest(url);
	request.setRawHeader("User-Agent", "SereinGram");
	request.setTransferTimeout(kRequestTimeout);
	const auto reply = network->get(request);
	QObject::connect(reply, &QNetworkReply::finished, network, [=] {
		reply->deleteLater();
		network->deleteLater();
		const auto body = reply->read(kMaximumSize + 1);
		if (reply->error() != QNetworkReply::NoError
			|| body.size() > kMaximumSize) {
			Notify(show, tr::lng_serein_filter_subscription_failed(tr::now));
			return;
		}
		Store(show, body);
	});
}

void StartRuleSubscription() {
	const auto timer = new QTimer(&Core::App());
	QObject::connect(timer, &QTimer::timeout, timer, [=] {
		timer->setInterval(kUpdateInterval);
		if (!ForDevice().Get(kRuleSubscription).isEmpty()) {
			UpdateRuleSubscription(nullptr);
		}
	});
	timer->start(kFirstUpdateDelay);
}

} // namespace Serein::Filters
