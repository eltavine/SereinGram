#pragma once

#include "api/api_transcribes.h"

#include <gsl/pointers>

class HistoryItem;

namespace Main {
class Session;
class SessionShow;
} // namespace Main

namespace Nagram {

void ShowCustomTranscription(
    std::shared_ptr<Main::SessionShow> show,
    not_null<HistoryItem*> item,
    bool manage = false);

[[nodiscard]] bool ExternalTranscriptionSelected(
	not_null<Main::Session*> session);

// Non-null while an external service replaces Telegram transcriptions.
[[nodiscard]] const Api::Transcribes::Entry *TranscriptionOverride(
	not_null<HistoryItem*> item);

} // namespace Nagram
