#include "serein/filters/subscription.h"
#include "serein/adapters/qtnetwork/manager.h"

#include "serein/filters/model.h"
#include "core/application.h"
#include "lang/lang_keys.h"
#include "ui/layers/show.h"

#include <QtCore/QTimer>

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
	Adapters::Download({
		.url = url,
		.maximumSize = kMaximumSize,
		.timeout = kRequestTimeout,
	}, [=](std::optional<QByteArray> body) {
		if (!body) {
			Notify(show, tr::lng_serein_filter_subscription_failed(tr::now));
			return;
		}
		Store(show, *body);
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
