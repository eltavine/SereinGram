#pragma once

namespace Main {
class SessionShow;
} // namespace Main

namespace Serein::Network {

void SortProxiesByLatency(std::shared_ptr<Main::SessionShow> show);
void RemoveUnavailableProxies(std::shared_ptr<Main::SessionShow> show);

} // namespace Serein::Network
