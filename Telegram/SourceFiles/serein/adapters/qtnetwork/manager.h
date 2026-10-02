#pragma once

#include "base/basic_types.h"

#include <QtCore/QByteArray>
#include <QtCore/QUrl>

#include <optional>
#include <utility>
#include <vector>

class QNetworkAccessManager;
class QNetworkReply;

namespace Serein::Adapters {

struct DownloadRequest {
	QUrl url;
	qint64 maximumSize = 0;
	int timeout = 0;
	std::vector<std::pair<QByteArray, QByteArray>> headers;
};

[[nodiscard]] QNetworkAccessManager &SharedNetwork();

QNetworkReply *Download(
	const DownloadRequest &request,
	Fn<void(std::optional<QByteArray> body)> done);

} // namespace Serein::Adapters
