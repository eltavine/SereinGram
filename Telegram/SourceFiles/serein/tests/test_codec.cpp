#include "serein/schema/gen/history/record.h"
#include "base/basic_types.h"

#include <iostream>
#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

[[nodiscard]] Serein::History::Record Sample() {
	using namespace Serein::History;
	auto result = Record();
	result.kind = RecordKind::Edited;
	result.peerId = -1001234567890123;
	result.messageId = 42;
	result.revision = 3;
	result.date = 1759240000;
	result.text = u"hello, world"_q;
	result.entities = { TextEntity{ u"bold"_q, 0, 5, QString() } };
	result.apiLayer = 214;
	result.tlMessage = QByteArray("\x01\x02\xff", 3);
	return result;
}

[[nodiscard]] bool Rejects(const QByteArray &raw, const char *path) {
	auto error = Serein::Codec::Error();
	const auto parsed = Serein::History::ParseRecord(raw, &error);
	return !parsed && error.path == QString::fromLatin1(path);
}

} // namespace

void TestCodec() {
	using namespace Serein::History;
	const auto raw = SerializeRecord(Sample());
	Require(raw.contains("\"version\":1"), "document version is written");
	Require(raw.contains("\"peerId\":\"-1001234567890123\""),
		"int64 fields are written as strings");
	Require(raw.contains("\"tlMessage\":\"AQL/\""), "bytes are written as base64");
	Require(raw.contains("\"kind\":\"RECORD_KIND_EDITED\""),
		"enums are written by name");
	const auto parsed = ParseRecord(raw);
	Require(parsed && *parsed == Sample(), "record survives a round trip");

	auto numeric = raw;
	numeric.replace("\"messageId\":\"42\"", "\"messageId\":42");
	Require(ParseRecord(numeric).has_value(), "int64 accepts JSON numbers");

	Require(Rejects(QByteArray(raw).replace("\"version\":1", "\"version\":2"), "version"),
		"unknown versions are rejected");
	Require(Rejects(QByteArray(raw).replace("{\"apiLayer\"", "{\"extra\":1,\"apiLayer\""), "$"),
		"unknown fields are rejected");
	Require(Rejects(QByteArray(raw).replace("RECORD_KIND_EDITED", "RECORD_KIND_UNSPECIFIED"), "kind"),
		"schema rules reject an unspecified kind");
	Require(Rejects(QByteArray(raw).replace("\"type\":\"bold\"", "\"type\":\"Bold\""), "entities[0].type"),
		"nested rules report the item path");
	Require(Rejects(QByteArray(raw).replace("\"revision\":3", "\"revision\":3.5"), "revision"),
		"fractional numbers are not 32-bit integers");
	Require(Rejects("[]", "$"), "documents must be JSON objects");
	std::cout << "PASS: Serein schema codecs" << std::endl;
}
