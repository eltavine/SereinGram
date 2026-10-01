#include "serein/network/proxy_notes.h"

#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestProxyNotes() {
	using namespace Serein::Network;
	Require(ProxyNoteKey(u" Proxy.Example.COM "_q, 443) == u"proxy.example.com:443"_q,
		"proxy note key is not normalized");
	Require(ParseProxyNotes(QByteArray()) == ProxyNotes(),
		"empty proxy notes are not an empty map");
	const auto notes = ProxyNotes{
		{ u"proxy.example.com:443"_q, u"Home"_q },
		{ u"10.0.0.1:1080"_q, u"Office VPN"_q },
	};
	const auto raw = SerializeProxyNotes(notes);
	Require(ParseProxyNotes(raw) == notes, "proxy notes do not round trip");
	Require(SerializeProxyNotes({}).isEmpty(), "no notes do not serialize empty");
	Require(ValidProxyNote(u"Home"_q) && !ValidProxyNote(QString())
		&& !ValidProxyNote(u" padded"_q)
		&& !ValidProxyNote(QString(kMaxProxyNoteLength + 1, u'a'))
		&& !ValidProxyNote(u"line\nbreak"_q), "proxy note validation");
	for (const auto bad : {
		R"([])",
		R"({})",
		R"({"Host:1":"x"})",
		R"({"host":"x"})",
		R"({"host:1":1})",
		R"({"host:1":""})",
		R"({"host:1":"bad\u0007"})",
		R"(not json)",
	}) {
		Require(!ParseProxyNotes(bad), "malformed proxy notes accepted");
	}
	auto many = QByteArray("{");
	for (auto i = 0; i <= kMaxProxyNotes; ++i) {
		many += (i ? "," : "") + ("\"h:" + QByteArray::number(i) + "\":\"n\"");
	}
	many += '}';
	Require(!ParseProxyNotes(many), "too many proxy notes accepted");
}
