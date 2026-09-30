#include "serein/hooks/services/model.h"
#include "serein/services/translation_protocol.h"
#include "base/basic_types.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QRegularExpression>

#include <iostream>
#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestServices() {
	using namespace Serein;
	const auto service = ServiceDefinition{
		.id = u"00000000-0000-0000-0000-000000000001"_q,
		.name = u"Local translation"_q,
		.kind = ServiceKind::Translation,
		.protocol = u"openai"_q,
		.baseUrl = QUrl(u"http://127.0.0.1:18765/v1/"_q),
		.endpoint = u"chat/completions"_q,
		.model = u"stub"_q,
		.credentialRef = u"00000000-0000-0000-0000-000000000002"_q,
	};
	const auto serialized = SerializeService(service);
	Require(ParseService(serialized).has_value(), "valid local service rejected");
	Require(!serialized.contains(u"apiKey"_q)
		&& !serialized.contains(u"secret"_q), "secret serialized");
	auto config = ServicesDefaults();
	config.insert(u"instances"_q, QJsonArray{ serialized });
	config.insert(u"translation"_q, service.id);
	Require(ValidServices(config), "valid service selection rejected");
	Require(ValidServicesBytes(QJsonDocument(config).toJson()),
		"valid service bytes rejected");
	const auto account = CredentialAccount(service);
	auto changed = service;
	changed.endpoint = u"other"_q;
	Require(CredentialAccount(changed) != account,
		"credential binding ignored endpoint");
	auto invalid = serialized;
	invalid.insert(u"baseUrl"_q, u"http://example.com/v1/"_q);
	Require(!ParseService(invalid), "remote HTTP accepted");
	invalid = serialized;
	invalid.insert(u"endpoint"_q, u"../other"_q);
	Require(!ParseService(invalid), "path traversal accepted");
	invalid = serialized;
	invalid.insert(u"apiKey"_q, u"must-not-store"_q);
	Require(!ParseService(invalid), "secret field accepted");
	config.insert(u"transcription"_q, service.id);
	Require(!ValidServices(config), "cross-kind selection accepted");
	Require(!ValidServicesBytes("{broken"), "invalid JSON accepted");
	invalid = serialized;
	invalid.remove(u"temperature"_q);
	Require(!ParseService(invalid), "service without temperature accepted");
	invalid = serialized;
	invalid.insert(u"useKey"_q, 1);
	Require(!ParseService(invalid), "numeric useKey accepted");
	invalid = serialized;
	invalid.insert(u"temperature"_q, u"0.5"_q);
	Require(!ParseService(invalid), "string temperature accepted");
	invalid = serialized;
	invalid.insert(u"temperature"_q, 2.5);
	Require(!ParseService(invalid), "temperature above 2 accepted");
	invalid = serialized;
	invalid.insert(u"kind"_q, u"summary"_q);
	Require(!ParseService(invalid), "unknown service kind accepted");
	invalid = serialized;
	invalid.insert(u"id"_q, u"00000000-0000-0000-0000-000000000000"_q);
	Require(!ParseService(invalid), "nil service id accepted");
	invalid = serialized;
	invalid.insert(u"language"_q, u"eng"_q);
	Require(!ParseService(invalid), "three letter language accepted");
	auto withTemperature = serialized;
	withTemperature.insert(u"temperature"_q, 0.25);
	const auto parsedTemperature = ParseService(withTemperature);
	Require(parsedTemperature && parsedTemperature->temperature == 0.25,
		"temperature not parsed");
	Require(SerializeService(*parsedTemperature) == withTemperature,
		"service does not round trip");
	auto duplicate = ServicesDefaults();
	duplicate.insert(u"instances"_q, QJsonArray{ serialized, serialized });
	Require(!ValidServices(duplicate), "duplicate service ids accepted");
	auto missingSelection = ServicesDefaults();
	missingSelection.insert(u"translation"_q, u"00000000-0000-0000-0000-000000000009"_q);
	Require(!ValidServices(missingSelection), "unknown selected service accepted");
	auto systemSelection = ServicesDefaults();
	systemSelection.insert(u"translation"_q, u"system"_q);
	Require(ValidServices(systemSelection), "system translation rejected");
	systemSelection.insert(u"transcription"_q, u"system"_q);
	Require(!ValidServices(systemSelection), "system transcription accepted");
	auto extraRoot = ServicesDefaults();
	extraRoot.insert(u"extra"_q, 1);
	Require(!ValidServices(extraRoot), "services config with an unknown key accepted");
	std::cout << "PASS: Serein service config and credential binding" << std::endl;
}

void TestTranslationProtocols() {
	using namespace Serein;
	const auto chat = ServiceDefinition{
		.id = u"00000000-0000-0000-0000-000000000001"_q,
		.name = u"Chat"_q,
		.protocol = u"openai"_q,
		.baseUrl = QUrl(u"https://api.example.com/v1/"_q),
		.endpoint = u"chat/completions"_q,
		.model = u"stub"_q,
		.credentialRef = u"00000000-0000-0000-0000-000000000002"_q,
	};
	const auto google = ServiceDefinition{
		.id = u"00000000-0000-0000-0000-000000000003"_q,
		.name = u"Google"_q,
		.protocol = u"google"_q,
		.baseUrl = QUrl(u"https://translate.googleapis.com/"_q),
		.endpoint = u"translate_a/single"_q,
		.credentialRef = u"00000000-0000-0000-0000-000000000004"_q,
		.useKey = false,
	};
	Require(ParseService(SerializeService(google)).has_value(),
		"google service rejected");
	auto changed = google;
	changed.useKey = true;
	Require(!ParseService(SerializeService(changed)), "keyed google accepted");
	changed = google;
	changed.kind = ServiceKind::Transcription;
	Require(!ParseService(SerializeService(changed)),
		"google transcription accepted");
	changed = google;
	changed.prompt = u"be brief"_q;
	Require(!ParseService(SerializeService(changed)), "google prompt accepted");
	Require(TranslationBatchLimit(google) == 1
		&& TranslationBatchLimit(chat) == 50, "wrong batch limits");

	const auto call = BuildTranslationCall(google, { u"C++ & you"_q }, u"zh"_q);
	Require(call.form == QByteArray("q=C%2B%2B%20%26%20you"),
		"google form body not percent-encoded");
	Require(call.query.queryItemValue(u"tl"_q) == u"zh"_q
		&& call.query.queryItemValue(u"dj"_q) == u"1"_q, "wrong google query");
	const auto joined = ParseTranslationResponse(google, QByteArray(R"({
		"sentences": [
			{ "trans": "Hallo, ", "orig": "Hello, " },
			{ "trans": "Welt", "orig": "world" }
		],
		"src": "en"
	})"), 1);
	Require(joined && joined->front() == u"Hallo, Welt"_q,
		"google sentences not joined");
	Require(!ParseTranslationResponse(google, "{}", 1),
		"empty google response accepted");

	const auto batch = BuildTranslationCall(chat, { u"a"_q, u"b"_q }, u"de"_q);
	Require(!batch.form && batch.json.value(u"model"_q) == u"stub"_q,
		"wrong chat body");
	const auto reply = QByteArray(R"({"choices": [{
		"finish_reason": "stop",
		"message": { "content": "[\"x\", \"y\"]" }
	}]})");
	const auto parsed = ParseTranslationResponse(chat, reply, 2);
	Require(parsed && *parsed == QStringList{ u"x"_q, u"y"_q },
		"chat response not parsed");
	Require(!ParseTranslationResponse(chat, reply, 3),
		"chat count mismatch accepted");

	auto deepl = chat;
	deepl.protocol = u"deepl"_q;
	const auto deeplCall = BuildTranslationCall(deepl, { u"a"_q }, u"de"_q);
	Require(deeplCall.json.value(u"target_lang"_q) == u"DE"_q,
		"wrong deepl body");
	const auto deeplParsed = ParseTranslationResponse(
		deepl,
		R"({"translations": [{ "text": "b" }]})",
		1);
	Require(deeplParsed && deeplParsed->front() == u"b"_q,
		"deepl response not parsed");

	auto yandex = google;
	yandex.protocol = u"yandex"_q;
	yandex.baseUrl = QUrl(u"https://translate.yandex.net/api/v1/tr.json/"_q);
	yandex.endpoint = u"translate"_q;
	Require(ParseService(SerializeService(yandex)).has_value(),
		"yandex service rejected");
	const auto yandexCall = BuildTranslationCall(
		yandex,
		{ u"a b"_q, u"C+"_q },
		u"zh"_q);
	Require(yandexCall.form == QByteArray("lang=zh&text=a%20b&text=C%2B"),
		"wrong yandex form body");
	static const auto yandexId = QRegularExpression(u"\\A[0-9a-f]{32}-0-0\\z"_q);
	Require(yandexId.match(yandexCall.query.queryItemValue(u"id"_q)).hasMatch()
		&& yandexCall.query.queryItemValue(u"srv"_q) == u"android"_q,
		"wrong yandex query");
	const auto yandexParsed = ParseTranslationResponse(
		yandex,
		R"({"code": 200, "lang": "en-zh", "text": ["x", "y"]})",
		2);
	Require(yandexParsed && yandexParsed->back() == u"y"_q,
		"yandex response not parsed");
	Require(!ParseTranslationResponse(
		yandex,
		R"({"code": 403, "text": ["x", "y"]})",
		2), "failed yandex response accepted");

	auto deeplx = chat;
	deeplx.protocol = u"deeplx"_q;
	deeplx.model = QString();
	deeplx.baseUrl = QUrl(u"http://127.0.0.1:1188/"_q);
	deeplx.endpoint = u"translate"_q;
	deeplx.useKey = false;
	Require(ParseService(SerializeService(deeplx)).has_value(),
		"deeplx service rejected");
	Require(TranslationBatchLimit(deeplx) == 1, "deeplx batches texts");
	const auto deeplxCall = BuildTranslationCall(deeplx, { u"a"_q }, u"zh"_q);
	Require(deeplxCall.json.value(u"text"_q) == u"a"_q
		&& deeplxCall.json.value(u"target_lang"_q) == u"ZH"_q,
		"wrong deeplx body");
	const auto deeplxParsed = ParseTranslationResponse(
		deeplx,
		R"({"code": 200, "data": "b"})",
		1);
	Require(deeplxParsed && deeplxParsed->front() == u"b"_q,
		"deeplx response not parsed");
	changed = deeplx;
	changed.model = u"stub"_q;
	Require(!ParseService(SerializeService(changed)), "deeplx model accepted");

	auto anthropic = chat;
	anthropic.protocol = u"anthropic"_q;
	anthropic.endpoint = u"messages"_q;
	anthropic.systemPrompt = u"You translate."_q;
	anthropic.temperature = 0.5;
	Require(ParseService(SerializeService(anthropic)).has_value(),
		"anthropic service rejected");
	changed = anthropic;
	changed.temperature = 1.5;
	Require(!ParseService(SerializeService(changed)),
		"anthropic temperature above 1 accepted");
	const auto anthropicCall = BuildTranslationCall(
		anthropic,
		{ u"a"_q },
		u"de"_q);
	Require(anthropicCall.json.value(u"max_tokens"_q).toInt() > 0
		&& anthropicCall.json.value(u"system"_q) == u"You translate."_q
		&& anthropicCall.json.value(u"messages"_q).toArray().size() == 1,
		"wrong anthropic body");
	const auto anthropicReply = QByteArray(R"({
		"content": [{ "type": "text", "text": "[\"b\"]" }],
		"stop_reason": "end_turn"
	})");
	const auto anthropicParsed = ParseTranslationResponse(
		anthropic,
		anthropicReply,
		1);
	Require(anthropicParsed && anthropicParsed->front() == u"b"_q,
		"anthropic response not parsed");
	auto truncated = anthropicReply;
	truncated.replace("end_turn", "max_tokens");
	Require(!ParseTranslationResponse(anthropic, truncated, 1),
		"truncated anthropic response accepted");

	const auto headers = ServiceHeaders(anthropic);
	Require(headers.size() == 1 && headers.front().first == "anthropic-version",
		"missing anthropic version header");
	Require(ServiceHeaders(chat).empty(), "unexpected chat headers");
	Require(ServiceAuthorization(anthropic, "k").first == "x-api-key",
		"wrong anthropic auth header");
	Require(ServiceAuthorization(deepl, "k").second == "DeepL-Auth-Key k",
		"wrong deepl auth header");
	Require(ServiceAuthorization(chat, "k").second == "Bearer k",
		"wrong bearer auth header");
	Require(IsKeylessProtocol(u"yandex"_q) && !IsKeylessProtocol(u"deeplx"_q),
		"wrong keyless protocols");

	auto transmart = yandex;
	transmart.protocol = u"transmart"_q;
	transmart.baseUrl = QUrl(u"https://transmart.qq.com/"_q);
	transmart.endpoint = u"api/imt"_q;
	Require(ParseService(SerializeService(transmart)).has_value(),
		"transmart service rejected");
	const auto transmartCall = BuildTranslationCall(
		transmart,
		{ u"a"_q, u"b"_q },
		u"zh"_q);
	const auto source = transmartCall.json.value(u"source"_q).toObject();
	Require(!transmartCall.form
		&& source.value(u"lang"_q) == u"auto"_q
		&& source.value(u"text_list"_q).toArray().size() == 2
		&& transmartCall.json.value(u"target"_q).toObject()
			.value(u"lang"_q) == u"zh"_q
		&& transmartCall.json.value(u"header"_q).toObject()
			.value(u"client_key"_q).toString().startsWith(u"browser-"_q),
		"wrong transmart body");
	const auto transmartParsed = ParseTranslationResponse(transmart, R"({
		"header": { "type": "auto_translation", "ret_code": "succ" },
		"auto_translation": ["x", "y"]
	})", 2);
	Require(transmartParsed && transmartParsed->front() == u"x"_q,
		"transmart response not parsed");
	Require(!ParseTranslationResponse(transmart, R"({
		"header": { "ret_code": "error" },
		"auto_translation": ["x", "y"]
	})", 2), "failed transmart response accepted");
	std::cout << "PASS: Serein translation protocols" << std::endl;
}
