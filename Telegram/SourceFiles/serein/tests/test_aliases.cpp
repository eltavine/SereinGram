#include "serein/privacy/alias_rules.h"
#include "serein/schema/gen/config/aliases.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestAliases() {
	using namespace Serein::Privacy;
	const auto parsed = ParsePeerAliasesConfig(
		R"({"version":1,"names":{"777000":"Service","42":"Friend"}})");
	Require(parsed && parsed->names.size() == 2
		&& parsed->names.at(QString::fromLatin1("42")) == QString::fromLatin1("Friend"),
		"alias config not parsed");
	Require(ParsePeerAliasesConfig(SerializePeerAliasesConfig(*parsed)) == parsed,
		"alias config does not round trip");
	for (const auto bad : {
		R"({"version":2,"names":{}})",
		R"({"version":1})",
		R"({"version":1,"names":{},"extra":1})",
		R"({"version":1,"names":{"0":"Zero"}})",
		R"({"version":1,"names":{"042":"Padded"}})",
		R"({"version":1,"names":{"42":""}})",
		R"({"version":1,"names":{"42":" padded"}})",
		R"({"version":1,"names":{"42":"line\nbreak"}})",
	}) {
		Require(!ParsePeerAliasesConfig(bad), "malformed alias config accepted");
	}
	auto many = PeerAliasesConfig();
	for (auto i = 1; i <= 1001; ++i) {
		many.names.emplace(QString::number(i), QString::fromLatin1("Name"));
	}
	Require(!ParsePeerAliasesConfig(SerializePeerAliasesConfig(many)),
		"too many aliases accepted");
	Require(ValidAlias(QString::fromUtf8("\u5C0F\u660E"))
		&& !ValidAlias(QString(97, QChar(u'a'))),
		"alias length rules");
}
