#pragma once

#include <memory>

namespace Ui {
class Show;
} // namespace Ui

namespace Serein::Network {

void UpdateProxySubscription(std::shared_ptr<Ui::Show> show);

} // namespace Serein::Network
