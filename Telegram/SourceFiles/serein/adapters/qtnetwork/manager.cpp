#include "serein/adapters/qtnetwork/manager.h"

#include "base/assertion.h"

#include <QtCore/QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Serein::Adapters {

QNetworkAccessManager &SharedNetwork() {
	Expects(QCoreApplication::instance() != nullptr);

	static const auto result = new QNetworkAccessManager(
		QCoreApplication::instance());
	return *result;
}

QNetworkReply *Download(
		const DownloadRequest &request,
		Fn<void(std::optional<QByteArray> body)> done) {
	auto network = QNetworkRequest(request.url);
	network.setRawHeader("User-Agent", "SereinGram");
	for (const auto &[name, value] : request.headers) {
		network.setRawHeader(name, value);
	}
	network.setTransferTimeout(request.timeout);
	const auto reply = SharedNetwork().get(network);
	const auto maximum = request.maximumSize;
	QObject::connect(reply, &QNetworkReply::finished, reply, [=] {
		reply->deleteLater();
		auto body = reply->read(maximum + 1);
		if (reply->error() != QNetworkReply::NoError
			|| body.size() > maximum) {
			done(std::nullopt);
		} else {
			done(std::move(body));
		}
	});
	return reply;
}

} // namespace Serein::Adapters
