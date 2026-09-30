#include "serein/services/model.h"
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
