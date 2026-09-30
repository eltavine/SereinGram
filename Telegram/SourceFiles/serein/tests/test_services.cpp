#include "serein/services/model.h"
#include "serein/services/translation_protocol.h"
#include "base/basic_types.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

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
	std::cout << "PASS: Serein translation protocols" << std::endl;
}
