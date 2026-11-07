#pragma once

#include "base/basic_types.h"

#include <gsl/pointers>

#include <memory>

class HistoryItem;

namespace Data {
class DocumentMedia;
} // namespace Data

namespace Serein {

class ServiceRequest;

enum class ExternalTranscription {
	Started,
	Downloading,
	Unavailable,
};

[[nodiscard]] ExternalTranscription TranscribeExternally(
	gsl::not_null<HistoryItem*> item,
	const std::shared_ptr<Data::DocumentMedia> &media,
	ServiceRequest &request,
	Fn<void(bool stored)> done);

} // namespace Serein
