#include "serein/services/translation.h"

#include "core/application.h"
#include "data/data_peer_values.h"
#include "history/history.h"
#include "lang/translate_mtproto_provider.h"
#include "lang/translate_provider.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "serein/core/options.h"
#include "serein/hooks/history.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/services/request.h"
#include "serein/services/translation_context.h"
#include "serein/services/translation_protocol.h"
#include "base/flat_map.h"
#include "base/flat_set.h"
#include "platform/platform_translate_provider.h"
#include "ui/text/text_utilities.h"

namespace Serein {
namespace {

bool Protected(EntityType type) {
	switch (type) {
	case EntityType::Bold:
	case EntityType::Semibold:
	case EntityType::Italic:
	case EntityType::Underline:
	case EntityType::StrikeOut:
	case EntityType::Blockquote:
	case EntityType::Spoiler:
	case EntityType::Subscript:
	case EntityType::Superscript:
	case EntityType::Marked:
	case EntityType::Colorized:
		return false;
	default:
		return true;
	}
}

class ExternalTranslateProvider final : public Ui::TranslateProvider {
public:
	ExternalTranslateProvider(
		Main::Session *session,
		std::optional<ServiceDefinition> service,
		Fn<void(QString)> error,
		QString unavailable = QString())
	: _session(session)
	, _service(std::move(service))
	, _error(std::move(error))
	, _unavailable(std::move(unavailable)) {
	}

	bool supportsMessageId() const override {
		return false;
	}

	void request(
			Ui::TranslateProviderRequest request,
			LanguageId to,
			Fn<void(Ui::TranslateProviderResult)> done) override {
		_request.cancel();
		if (!_service) {
			fail(ServiceError::Configuration, 0, std::move(done));
			return;
		}
		auto plan = PlanTranslation(std::move(request.text));
		if (!plan || !to.known()) {
			fail(ServiceError::Response, 0, std::move(done));
			return;
		}
		_context = (_session
			&& SupportsTranslationContext(*_service)
			&& ForDevice().Get(ServiceSettings::kTranslationContext))
			? TranslationContext(_session, request.peerId, request.msgId)
			: QStringList();
		_plan = std::move(*plan);
		_translated.clear();
		_to = to.twoLetterCode();
		_done = std::move(done);
		next();
	}

	void requestBatch(
			std::vector<Ui::TranslateProviderRequest> requests,
			const LanguageId &to,
			Fn<void(int, Ui::TranslateProviderResult)> doneOne,
			Fn<void()> doneAll) override {
		_request.cancel();
		auto batch = Batch{
			.doneOne = std::move(doneOne),
			.doneAll = std::move(doneAll),
		};
		for (auto &request : requests) {
			auto plan = (_service && to.known())
				? PlanTranslation(std::move(request.text))
				: std::nullopt;
			batch.offsets.push_back(int(batch.texts.size()));
			if (plan) {
				batch.texts += plan->texts;
			}
			batch.plans.push_back(std::move(plan));
		}
		_batch = std::move(batch);
		_to = to.twoLetterCode();
		nextInBatch();
	}

private:
	struct Batch {
		std::vector<std::optional<TranslationPlan>> plans;
		std::vector<int> offsets;
		QStringList texts;
		QStringList translated;
		Fn<void(int, Ui::TranslateProviderResult)> doneOne;
		Fn<void()> doneAll;
	};

	void nextInBatch() {
		const auto offset = int(_batch.translated.size());
		if (offset == _batch.texts.size()) {
			finishBatch();
			return;
		}
		const auto amount = std::min(
			TranslationBatchLimit(*_service),
			int(_batch.texts.size()) - offset);
		const auto call = BuildTranslationCall(
			*_service,
			_batch.texts.mid(offset, amount),
			_to);
		_request.translate(*_service, call, [=](ServiceResult response) {
			const auto values = (response.error == ServiceError::None)
				? ParseTranslationResponse(*_service, response.body, amount)
				: std::nullopt;
			if (!values) {
				finishBatch();
				return;
			}
			_batch.translated += *values;
			nextInBatch();
		});
	}

	void finishBatch() {
		auto batch = std::exchange(_batch, Batch());
		for (auto i = 0; i != int(batch.plans.size()); ++i) {
			const auto &plan = batch.plans[i];
			const auto offset = batch.offsets[i];
			const auto count = plan ? int(plan->texts.size()) : 0;
			const auto result = (plan && offset + count <= batch.translated.size())
				? ApplyTranslation(*plan, batch.translated.mid(offset, count))
				: std::nullopt;
			batch.doneOne(i, result
				? Ui::TranslateProviderResult{ .text = *result }
				: Ui::TranslateProviderResult{
					.error = Ui::TranslateProviderError::Unknown,
				});
		}
		batch.doneAll();
	}

	void fail(ServiceError error, int status, Fn<void(Ui::TranslateProviderResult)> done) {
		const auto notify = _error;
		notify(_unavailable.isEmpty()
			? ServiceErrorText(error, status) : _unavailable);
		done({ .error = Ui::TranslateProviderError::Unknown });
	}

	void next() {
		const auto offset = int(_translated.size());
		if (offset == _plan.texts.size()) {
			const auto result = ApplyTranslation(_plan, _translated);
			const auto done = std::move(_done);
			if (!result) {
				fail(ServiceError::Response, 0, done);
			} else {
				done({ .text = *result });
			}
			return;
		}
		const auto amount = std::min(
			TranslationBatchLimit(*_service),
			int(_plan.texts.size()) - offset);
		const auto call = BuildTranslationCall(
			*_service,
			_plan.texts.mid(offset, amount),
			_to,
			_context);
		_request.translate(*_service, call, [=](ServiceResult response) {
			if (response.error != ServiceError::None) {
				fail(response.error, response.status, std::move(_done));
				return;
			}
			const auto values = ParseTranslationResponse(
				*_service,
				response.body,
				amount);
			if (!values) {
				fail(ServiceError::Response, 0, std::move(_done));
				return;
			}
			_translated += *values;
			next();
		});
	}

	Main::Session *_session = nullptr;
	std::optional<ServiceDefinition> _service;
	Fn<void(QString)> _error;
	QString _unavailable;
	Fn<void(Ui::TranslateProviderResult)> _done;
	ServiceRequest _request;
	TranslationPlan _plan;
	QStringList _translated;
	QStringList _context;
	QString _to;
	Batch _batch;

};

} // namespace

std::optional<TranslationPlan> PlanTranslation(TextWithEntities text) {
	const auto length = int(text.text.size());
	if (length > 1024 * 1024 || text.text.contains(QChar(0))
		|| QString::fromUtf8(text.text.toUtf8()) != text.text) {
		return std::nullopt;
	}
	auto boundaries = base::flat_set<int>{ 0, length };
	auto protection = std::vector<int>(length + 1);
	const auto collect = [&](const EntitiesInText &entities) {
		for (const auto &entity : entities) {
			if (!entity.validForText(length)) {
				return false;
			}
			const auto start = entity.offset();
			const auto end = start + entity.length();
			if ((start && text.text[start].isLowSurrogate())
				|| (end < length && text.text[end].isLowSurrogate())) {
				return false;
			}
			boundaries.emplace(start);
			boundaries.emplace(end);
			if (Protected(entity.type())) {
				++protection[start];
				--protection[end];
			}
		}
		return true;
	};
	if (!collect(text.entities)
		|| !collect(TextUtilities::ParseEntities(text.text,
			TextParseLinks | TextParseMentions | TextParseHashtags | TextParseBotCommands).entities)) {
		return std::nullopt;
	}
	auto codeStart = -1;
	auto codeTicks = 0;
	const auto protectCode = [&](int end) {
		boundaries.emplace(codeStart);
		boundaries.emplace(end);
		++protection[codeStart];
		--protection[end];
	};
	for (auto i = 0; i < length;) {
		if (text.text[i] == '\\' && codeStart < 0) {
			i += std::min(2, length - i);
		} else if (text.text[i] != '`') {
			++i;
		} else {
			const auto start = i;
			while (i < length && text.text[i] == '`') {
				++i;
			}
			const auto ticks = i - start;
			if (codeStart < 0) {
				codeStart = start;
				codeTicks = ticks;
			} else if (ticks == codeTicks) {
				protectCode(i);
				codeStart = -1;
			}
		}
	}
	if (codeStart >= 0) {
		protectCode(length);
	}
	auto active = 0;
	for (auto &value : protection) {
		active += value;
		value = active;
	}
	auto result = TranslationPlan{ .original = std::move(text) };
	auto previous = 0;
	for (const auto boundary : boundaries) {
		if (boundary == previous) {
			continue;
		}
		const auto part = result.original.text.mid(previous, boundary - previous);
		const auto translate = !protection[previous] && !part.trimmed().isEmpty();
		auto start = previous;
		auto end = boundary;
		if (translate) {
			while (start < end && result.original.text[start].isSpace()) {
				++start;
			}
			while (end > start && result.original.text[end - 1].isSpace()) {
				--end;
			}
		}
		if (start > previous) {
			result.parts.push_back({ previous, start - previous, -1 });
		}
		result.parts.push_back({
			.start = start,
			.length = end - start,
			.index = translate ? int(result.texts.size()) : -1,
		});
		if (translate) {
			result.texts.push_back(result.original.text.mid(start, end - start));
		}
		if (end < boundary) {
			result.parts.push_back({ end, boundary - end, -1 });
		}
		previous = boundary;
	}
	return result;
}

std::optional<TextWithEntities> ApplyTranslation(
		const TranslationPlan &plan,
		const QStringList &translated) {
	if (translated.size() != plan.texts.size()) {
		return std::nullopt;
	}
	auto boundaries = base::flat_map<int, int>();
	auto result = TextWithEntities();
	for (const auto &part : plan.parts) {
		boundaries[part.start] = result.text.size();
		const auto text = part.index < 0
			? plan.original.text.mid(part.start, part.length)
			: translated[part.index];
		if (text.isEmpty() || QString::fromUtf8(text.toUtf8()) != text
			|| text.contains(QChar(0)) || result.text.size() + text.size() > 1024 * 1024) {
			return std::nullopt;
		}
		result.text += text;
		boundaries[part.start + part.length] = result.text.size();
	}
	for (const auto &entity : plan.original.entities) {
		const auto start = boundaries.find(entity.offset());
		const auto end = boundaries.find(entity.offset() + entity.length());
		if (start == boundaries.end() || end == boundaries.end()) {
			return std::nullopt;
		}
		result.entities.push_back(EntityInText(
			entity.type(), start->second, end->second - start->second, entity.data()));
	}
	return result;
}

std::unique_ptr<Ui::TranslateProvider> CreateServiceTranslateProvider(
		const ServiceDefinition &service,
		Fn<void(QString)> error) {
	return std::make_unique<ExternalTranslateProvider>(
		nullptr,
		service.kind == ServiceKind::Translation
			&& ParseService(ServiceToInstance(service))
			? std::optional(service) : std::nullopt,
		std::move(error));
}

std::unique_ptr<Ui::TranslateProvider> CreateInteractiveTranslateProvider(
		not_null<Main::Session*> session,
		Fn<void(QString)> error) {
	const auto settings = Services();
	if (settings) {
		const auto &id = settings->translation;
		if (id.isEmpty()) {
			return Ui::CreateTranslateProvider(session);
		} else if (id == u"telegram"_q) {
			return Ui::CreateMTProtoTranslateProvider(session);
		} else if (id == u"system"_q) {
			if (Platform::IsTranslateProviderAvailable()) {
				return Platform::CreateTranslateProvider();
			}
			return std::make_unique<ExternalTranslateProvider>(
					session, std::nullopt, std::move(error),
					tr::lng_serein_system_translation_unavailable(tr::now));
		}
		return std::make_unique<ExternalTranslateProvider>(
			session, FindService(*settings, id), std::move(error));
	}
	return std::make_unique<ExternalTranslateProvider>(
		session, std::nullopt, std::move(error));
}

std::unique_ptr<Ui::TranslateProvider> CreateChatTranslateProvider(
		not_null<Main::Session*> session) {
	if (const auto settings = Services()) {
		const auto &id = settings->translation;
		if (id == u"telegram"_q) {
			return Ui::CreateMTProtoTranslateProvider(session);
		} else if (id == u"system"_q) {
			if (Platform::IsTranslateProviderAvailable()) {
				return Platform::CreateTranslateProvider();
			}
		} else if (!id.isEmpty()) {
			if (auto service = FindService(*settings, id)) {
				return std::make_unique<ExternalTranslateProvider>(
					session, std::move(service), [](QString) {});
			}
		}
	}
	return Ui::CreateTranslateProvider(session);
}

rpl::producer<bool> ChatTranslationAllowedValue(
		not_null<Main::Session*> session,
		not_null<Ui::TranslateProvider*> provider) {
	const auto external = !provider->supportsMessageId();
	return rpl::combine(
		Data::AmPremiumValue(session),
		ForDevice().Value(ServiceSettings::kChatTranslationWithoutPremium)
	) | rpl::map([=](bool premium, bool enabled) {
		return premium || (enabled && external);
	});
}

bool ChatTranslationAllowed(not_null<Main::Session*> session) {
	return session->premium()
		|| (ForDevice().Get(ServiceSettings::kChatTranslationWithoutPremium)
			&& !CreateChatTranslateProvider(session)->supportsMessageId());
}

bool Hooks::AutoTranslate(not_null<::History*> history) {
	const auto session = &history->session();
	return ForAccount(session).Get(Serein::ServiceSettings::kAutoTranslateChats)
		&& ChatTranslationAllowed(session);
}

} // namespace Serein
