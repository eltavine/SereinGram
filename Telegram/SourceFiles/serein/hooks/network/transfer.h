#pragma once

namespace Serein::Network {

[[nodiscard]] int DownloadStartSessions(int standard);
[[nodiscard]] int DownloadStartWindow(int standard, int maximum);
[[nodiscard]] int UploadSessionWindow(int standard);
[[nodiscard]] bool PreferLargeUploadParts();

} // namespace Serein::Network
