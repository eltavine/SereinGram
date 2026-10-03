#pragma once

#include "base/basic_types.h"

#include <gsl/pointers>

class HistoryItem;

namespace Serein {

class ServiceRequest;

enum class ExternalTranscription {
	Started,
	Downloading,
	Unavailable,
};

[[nodiscard]] ExternalTranscription TranscribeExternally(
	gsl::not_null<HistoryItem*> item,
	ServiceRequest &request,
	Fn<void(bool stored)> done);

} // namespace Serein
