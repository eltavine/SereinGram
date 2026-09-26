#pragma once

#include "base/basic_types.h"

#include <gsl/pointers>

#include <memory>

class DocumentData;

namespace Ui {
class Show;
} // namespace Ui

namespace Nagram::Compose {

// Returns true when a confirmation box took over the send.
[[nodiscard]] bool ConfirmBeforeSend(
	std::shared_ptr<Ui::Show> show,
	DocumentData *document,
	Fn<void()> resend);

} // namespace Nagram::Compose
