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

enum class Status {
	Unknown,
	Checking,
	UpToDate,
	Available,
	Failed,
	Managed,
};

struct UpdateState {
	Status status = Status::Unknown;
	std::optional<AvailableUpdate> update;

	friend inline bool operator==(
		const UpdateState &,
		const UpdateState &) = default;
};

void StartUpdateChecks();

[[nodiscard]] bool UpdateChecksAvailable();
[[nodiscard]] QString ReleasesUrl();
void CheckForUpdatesNow(Fn<void(CheckResult)> done);
[[nodiscard]] rpl::producer<UpdateState> StateValue();

} // namespace Serein::Updates
