#include "serein/adapters/qtnetwork/manager.h"

#include "base/assertion.h"

#include <QtCore/QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>

namespace Serein::Adapters {

QNetworkAccessManager &SharedNetwork() {
	Expects(QCoreApplication::instance() != nullptr);

	static const auto result = new QNetworkAccessManager(
		QCoreApplication::instance());
	return *result;
}

} // namespace Serein::Adapters
