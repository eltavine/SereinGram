#pragma once

class QNetworkAccessManager;

namespace Serein::Adapters {

[[nodiscard]] QNetworkAccessManager &SharedNetwork();

} // namespace Serein::Adapters
