#include "serein/services/auto_transcription.h"

#include "serein/core/options.h"
#include "serein/hooks/services/transcription.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/services/external_transcription.h"
#include "serein/services/request.h"
#include "api/api_transcribes.h"
#include "apiwrap.h"
#include "data/data_document.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

#include <deque>

namespace Serein {
namespace {

constexpr auto kMaxQueued = 32;
constexpr auto kMaxDownloadWaits = 5;

enum class Mode {
	Off = 0,
	PrivateChats = 1,
	AllChats = 2,
};

[[nodiscard]] bool Eligible(not_null<HistoryItem*> item) {
	const auto session = &item->history()->session();
	const auto mode = Mode(
		ForAccount(session).Get(ServiceSettings::kAutoTranscribe));
	const auto media = item->media();
	const auto document = media ? media->document() : nullptr;
	return (mode != Mode::Off)
		&& !item->out()
		&& item->isRegular()
		&& document
		&& (document->isVoiceMessage() || document->isVideoMessage())
		&& !media->ttlSeconds()
		&& (mode == Mode::AllChats || item->history()->peer->isUser());
}

class AutoTranscriber final {
public:
	explicit AutoTranscriber(not_null<Main::Session*> session);

private:
	void added(not_null<HistoryItem*> item);
	void next();

	const not_null<Main::Session*> _session;
	ServiceRequest _request;
	std::deque<FullMsgId> _queue;
	base::flat_map<FullMsgId, int> _attempts;
	FullMsgId _active;
	rpl::lifetime _lifetime;

};

AutoTranscriber::AutoTranscriber(not_null<Main::Session*> session)
: _session(session) {
	_session->data().newItemAdded(
	) | rpl::filter(Eligible) | rpl::on_next([=](not_null<HistoryItem*> item) {
		added(item);
	}, _lifetime);

	_session->downloaderTaskFinished(
	) | rpl::filter([=] {
		return !_active && !_queue.empty();
	}) | rpl::on_next([=] {
		next();
	}, _lifetime);
}

void AutoTranscriber::added(not_null<HistoryItem*> item) {
	if (!ExternalTranscriptionSelected(_session)) {
		auto &transcribes = _session->api().transcribes();
		const auto &entry = transcribes.entry(item);
		if ((_session->premium() || transcribes.freeFor(item))
			&& !entry.requestId
			&& entry.result.isEmpty()
			&& !entry.failed) {
			transcribes.toggle(item);
		}
		return;
	} else if (int(_queue.size()) >= kMaxQueued) {
		return;
	}
	_queue.push_back(item->fullId());
	next();
}

void AutoTranscriber::next() {
	auto downloading = std::deque<FullMsgId>();
	while (!_active && !_queue.empty()) {
		const auto id = _queue.front();
		_queue.pop_front();
		const auto item = _session->data().message(id);
		if (!item) {
			_attempts.remove(id);
			continue;
		}
		const auto result = TranscribeExternally(item, _request, [=](bool) {
			_active = FullMsgId();
			next();
		});
		if (result == ExternalTranscription::Started) {
			_attempts.remove(id);
			_active = id;
		} else if (result == ExternalTranscription::Downloading
			&& ++_attempts[id] <= kMaxDownloadWaits) {
			downloading.push_back(id);
		} else {
			_attempts.remove(id);
		}
	}
	_queue.insert(end(_queue), begin(downloading), end(downloading));
}

} // namespace

void WatchAutoTranscription(not_null<Main::Session*> session) {
	session->lifetime().make_state<AutoTranscriber>(session);
}

} // namespace Serein
