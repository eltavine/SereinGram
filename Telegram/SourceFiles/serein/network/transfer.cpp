#include "serein/hooks/network/transfer.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"

#include <algorithm>

namespace Serein::Network {
namespace {

constexpr auto kFastStartSessions = 4;
constexpr auto kFastUploadWindowFactor = 4;

[[nodiscard]] bool Faster() {
	return ForDevice().Get(ServiceSettings::kFasterTransfers);
}

} // namespace

int DownloadStartSessions(int standard) {
	return Faster() ? std::max(standard, kFastStartSessions) : standard;
}

int DownloadStartWindow(int standard, int maximum) {
	return Faster() ? maximum : standard;
}

int UploadSessionWindow(int standard) {
	return Faster() ? (standard * kFastUploadWindowFactor) : standard;
}

} // namespace Serein::Network
