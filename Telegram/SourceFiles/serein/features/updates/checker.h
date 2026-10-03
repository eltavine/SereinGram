#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <optional>

namespace Serein::Updates {

struct AvailableUpdate {
	QString id;
	QString version;
	QString page;
	QString download;

	friend inline bool operator==(
		const AvailableUpdate &,
		const AvailableUpdate &) = default;
};

enum class CheckResult {
	UpToDate,
	Available,
	Failed,
};

void StartUpdateChecks();

[[nodiscard]] bool UpdateChecksAvailable();
[[nodiscard]] QString ReleasesUrl();
void CheckForUpdatesNow(Fn<void(CheckResult)> done);
[[nodiscard]] rpl::producer<bool> CheckingValue();
[[nodiscard]] rpl::producer<std::optional<AvailableUpdate>> AvailableValue();

} // namespace Serein::Updates
