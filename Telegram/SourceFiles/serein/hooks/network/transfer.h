#pragma once

namespace Serein::Network {

[[nodiscard]] int DownloadStartSessions(int standard);
[[nodiscard]] int DownloadStartWindow(int standard, int maximum);
[[nodiscard]] int UploadSessionWindow(int standard);

} // namespace Serein::Network
