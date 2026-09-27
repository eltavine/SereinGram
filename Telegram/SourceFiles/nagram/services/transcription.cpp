#include "nagram/services/transcription.h"

#include "apiwrap.h"
#include "api/api_transcribes.h"
#include "data/data_document.h"
#include "data/data_file_origin.h"
#include "data/data_document_media.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "nagram/core/options.h"
#include "nagram/display/view_refresher.h"
#include "nagram/services/model.h"
#include "nagram/services/request.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/labels.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>

#include <map>
#include <memory>

#include "styles/style_layers.h"

namespace Nagram {
namespace {

constexpr auto kMaximumText = 16384;
constexpr auto kMaximumEntries = 128;

[[nodiscard]] bool TranscriptionServiceSelected() {
	const auto config = Services();
	if (!config) {
		return false;
	}
	const auto id = config->value(u"transcription"_q).toString();
	return !id.isEmpty() && id != u"telegram"_q;
}

class ExternalTranscriptions final {
public:
	explicit ExternalTranscriptions(not_null<Main::Session*> session);

	[[nodiscard]] static ExternalTranscriptions &For(
		not_null<Main::Session*> session);

	[[nodiscard]] bool selected() const {
		return _selected;
	}
	[[nodiscard]] const Api::Transcribes::Entry *find(
		not_null<HistoryItem*> item) const;
	[[nodiscard]] bool toggle(not_null<HistoryItem*> item);
	[[nodiscard]] bool set(
		not_null<HistoryItem*> item,
		DocumentId documentId,
		const QByteArray &serviceConfig,
		uint64 generation,
		QString result);
	[[nodiscard]] uint64 generation(bool round) const {
		return _generation[round ? 1 : 0];
	}

private:
	struct Cached {
		DocumentId documentId = 0;
		Api::Transcribes::Entry value;
		uint64 accessed = 0;
	};

	void refresh(FullMsgId id);
	void clear();

	const not_null<Main::Session*> _session;
	base::flat_map<FullMsgId, Cached> _entries;
	QByteArray _config;
	uint64 _accessed = 0;
	std::array<uint64, 2> _generation = {};
	bool _selected = false;
	rpl::lifetime _lifetime;
};

ExternalTranscriptions::ExternalTranscriptions(
	not_null<Main::Session*> session)
: _session(session)
, _config(ForDevice().Get(kServicesConfig))
, _selected(TranscriptionServiceSelected()) {
	ForDevice().Value(kServicesConfig) | rpl::on_next([=](
			const QByteArray &config) {
		if (_config == config) {
			return;
		}
		_config = config;
		_selected = TranscriptionServiceSelected();
		clear();
		ViewRefresher::Refresh(_session->data());
	}, _lifetime);
}

ExternalTranscriptions &ExternalTranscriptions::For(
		not_null<Main::Session*> session) {
	static auto states = std::map<
		not_null<Main::Session*>,
		std::unique_ptr<ExternalTranscriptions>>();
	if (const auto i = states.find(session); i != end(states)) {
		return *i->second;
	}
	const auto i = states.emplace(
		session,
		std::make_unique<ExternalTranscriptions>(session)).first;
	session->lifetime().add([=] { states.erase(session); });
	return *i->second;
}

const Api::Transcribes::Entry *ExternalTranscriptions::find(
		not_null<HistoryItem*> item) const {
	const auto i = _entries.find(item->fullId());
	const auto media = item->media();
	const auto document = media ? media->document() : nullptr;
	return (i != end(_entries)
		&& _selected
		&& document
		&& !media->ttlSeconds()
		&& document->id == i->second.documentId)
		? &i->second.value
		: nullptr;
}

bool ExternalTranscriptions::toggle(not_null<HistoryItem*> item) {
	if (!find(item)) {
		return false;
	}
	auto &cached = _entries[item->fullId()];
	cached.value.shown = !cached.value.shown;
	cached.accessed = ++_accessed;
	refresh(item->fullId());
	return true;
}

bool ExternalTranscriptions::set(
		not_null<HistoryItem*> item,
		DocumentId documentId,
		const QByteArray &serviceConfig,
		uint64 generation,
		QString result) {
	const auto media = item->media();
	const auto document = media ? media->document() : nullptr;
	if (&item->history()->session() != _session
		|| !_selected
		|| !document
		|| document->id != documentId
		|| (!document->isVoiceMessage() && !document->isVideoMessage())
		|| media->ttlSeconds()
		|| serviceConfig != _config
		|| generation != this->generation(document->isVideoMessage())
		|| result.isEmpty()
		|| result.size() > kMaximumText
		|| result.contains(QChar(0))
		|| QString::fromUtf8(result.toUtf8()) != result) {
		return false;
	}
	_entries[item->fullId()] = {
		.documentId = documentId,
		.value = {
			.result = std::move(result),
			.shown = true,
			.roundview = document->isVideoMessage(),
		},
		.accessed = ++_accessed,
	};
	if (_entries.size() > kMaximumEntries) {
		const auto oldest = ranges::min_element(
			_entries,
			ranges::less(),
			[](const auto &entry) { return entry.second.accessed; });
		const auto id = oldest->first;
		_entries.erase(oldest);
		refresh(id);
	}
	refresh(item->fullId());
	return true;
}

void ExternalTranscriptions::refresh(FullMsgId id) {
	if (const auto item = _session->data().message(id)) {
		_session->data().requestItemViewRefresh(item);
		_session->data().requestItemResize(item);
	}
}

void ExternalTranscriptions::clear() {
	for (auto &generation : _generation) {
		++generation;
	}
	auto removed = std::vector<FullMsgId>();
	for (const auto &[id, cached] : _entries) {
		removed.push_back(id);
	}
	_entries.clear();
	for (const auto id : removed) {
		refresh(id);
	}
}

} // namespace

bool ExternalTranscriptionSelected(not_null<Main::Session*> session) {
	return ExternalTranscriptions::For(session).selected();
}

const Api::Transcribes::Entry *TranscriptionOverride(
		not_null<HistoryItem*> item) {
	const auto &external = ExternalTranscriptions::For(
		&item->history()->session());
	if (!external.selected()) {
		return nullptr;
	}
	if (const auto found = external.find(item)) {
		return found;
	}
	static const auto empty = Api::Transcribes::Entry();
	return &empty;
}

void ShowCustomTranscription(
		std::shared_ptr<Main::SessionShow> show,
		not_null<HistoryItem*> item,
		bool manage) {
	const auto session = &show->session();
	if (!manage && ExternalTranscriptions::For(session).toggle(item)) {
		return;
	}
	const auto config = Services();
	const auto service = config
		? FindService(*config, config->value(u"transcription"_q).toString())
		: std::nullopt;
	const auto document = item->media() ? item->media()->document() : nullptr;
	if (!service || service->kind != ServiceKind::Transcription || !document) {
		show->showToast(tr::lng_nagram_service_invalid(tr::now));
		return;
	}
	const auto id = item->fullId();
	const auto documentId = document->id;
	const auto serviceConfig = ForDevice().Get(kServicesConfig);
	show->showBox(Box([=](not_null<Ui::GenericBox*> box) {
		const auto current = show->session().data().message(id);
		const auto currentMedia = current ? current->media() : nullptr;
		const auto currentDocument = currentMedia
			? currentMedia->document() : nullptr;
		if (!currentDocument || currentDocument->id != documentId
			|| currentMedia->ttlSeconds()) {
			box->addRow(object_ptr<Ui::FlatLabel>(box,
				tr::lng_nagram_transcribe_missing(), st::boxLabel));
			return;
		}
		struct State {
			ServiceRequest request;
			std::shared_ptr<Data::DocumentMedia> media;
			QString result;
			bool loading = false;
		};
		box->setTitle(tr::lng_nagram_service_transcription());
		const auto state = box->lifetime().make_state<State>();
		state->media = currentDocument->createMediaView();
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_nagram_transcribe_upload_about(
				lt_name, rpl::single(service->name),
				lt_url, rpl::single(ServiceEndpoint(*service).toDisplayString())),
			st::boxLabel));
		const auto label = box->addRow(object_ptr<Ui::FlatLabel>(box, st::boxLabel));
		label->setSelectable(current->allowsForward());
		state->result = show->session().api().transcribes().entry(current).result;
		label->setText(state->result);
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_nagram_transcribe_cache_about(),
			st::boxLabel));
		box->addButton(tr::lng_nagram_transcribe_start(), [=] {
			if (state->loading) {
				return;
			}
			if (ForDevice().Get(kServicesConfig)
				!= serviceConfig) {
				label->setText(tr::lng_nagram_service_invalid(tr::now));
				return;
			}
			const auto item = show->session().data().message(id);
			const auto media = item ? item->media() : nullptr;
			const auto document = media ? media->document() : nullptr;
			if (!document || document->id != documentId) {
				box->showToast(tr::lng_nagram_transcribe_missing(tr::now));
				return;
			}
			if (media->ttlSeconds()) {
				box->showToast(tr::lng_nagram_transcribe_missing(tr::now));
				return;
			}
			auto bytes = state->media->bytes();
			constexpr auto limit = 24 * 1024 * 1024;
			if (document->size > limit) {
				label->setText(ServiceErrorText(ServiceError::TooLarge));
				return;
			}
			if (bytes.isEmpty()) {
				const auto location = document->location(true);
				if (!location.isEmpty() && location.accessEnable()) {
					auto file = QFile(location.name());
					if (file.open(QIODevice::ReadOnly)) {
						bytes = file.read(limit + 1);
					}
					location.accessDisable();
				}
			}
			if (bytes.isEmpty()) {
				document->save(item->fullId(), QString());
				label->setText(tr::lng_nagram_transcribe_download(tr::now));
				return;
			}
			const auto generation = ExternalTranscriptions::For(
				session).generation(document->isVideoMessage());
			state->loading = true;
			label->setText(tr::lng_contacts_loading(tr::now));
			state->request.audio(*service, std::move(bytes),
				document->isVideoMessage() ? u"audio.mp4"_q : u"audio.ogg"_q,
				crl::guard(box, [=](ServiceResult response) {
					state->loading = false;
					if (response.error != ServiceError::None) {
						label->setText(ServiceErrorText(response.error, response.status));
						return;
					}
					const auto value = QJsonDocument::fromJson(
						response.body).object().value(u"text"_q);
					if (!value.isString() || value.toString().isEmpty()
						|| value.toString().size() > 16384
						|| value.toString().contains(QChar(0))) {
						label->setText(ServiceErrorText(ServiceError::Response));
						return;
					}
					const auto item = show->session().data().message(id);
					if (!item || !ExternalTranscriptions::For(session).set(
							item, documentId, serviceConfig, generation, value.toString())) {
						label->setText(tr::lng_nagram_transcribe_missing(tr::now));
						return;
					}
					state->result = value.toString();
					label->setText(state->result);
				}));
		});
		box->addButton(tr::lng_context_copy_text(), [=] {
			const auto item = show->session().data().message(id);
			const auto media = item ? item->media() : nullptr;
			const auto document = media ? media->document() : nullptr;
			if (item && item->allowsForward() && document
				&& document->id == documentId && !media->ttlSeconds()
				&& !state->result.isEmpty()
				&& show->session().api().transcribes().entry(item).result
					== state->result) {
				TextUtilities::SetClipboardText(TextForMimeData::Simple(state->result));
			}
		});
		box->addButton(tr::lng_cancel(), [=] {
			state->request.cancel();
			box->closeBox();
		});
	}));
}

} // namespace Nagram
