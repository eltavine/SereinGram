#pragma once

#include <gsl/pointers>

#include <QtCore/QString>

class PeerData;

namespace Serein::Privacy {

[[nodiscard]] QString PeerIdText(gsl::not_null<PeerData*> peer, bool botApi);

} // namespace Serein::Privacy
