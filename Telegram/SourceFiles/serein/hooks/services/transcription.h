#pragma once

#include <gsl/pointers>

#include <memory>

class HistoryItem;

namespace Main {
class Session;
class SessionShow;
} // namespace Main

namespace Serein {

void ShowCustomTranscription(
	std::shared_ptr<Main::SessionShow> show,
	gsl::not_null<HistoryItem*> item,
	bool manage = false);

[[nodiscard]] bool ExternalTranscriptionSelected(
	gsl::not_null<Main::Session*> session);

// Non-null while an external service replaces Telegram transcriptions.
template <typename Entry>
[[nodiscard]] const Entry *TranscriptionOverride(
	gsl::not_null<HistoryItem*> item);

} // namespace Serein
