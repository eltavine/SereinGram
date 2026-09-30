#include "serein/chats/local_pins.h"
#include "serein/chats/options.h"
#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestLocalPins() {
	using namespace Serein::Chats;
	auto value = QString();
	value = ToggleLocalPin(value, 7);
	value = ToggleLocalPin(value, 9);
	Require(value == u"7,9"_q, "local pins not appended in order");
	Require(ParseLocalPins(value) == std::vector<quint64>{ 7, 9 },
		"local pins not parsed");
	value = ToggleLocalPin(value, 7);
	Require(value == u"9"_q, "local pin not removed");
	for (auto id = quint64(100); id != 200; ++id) {
		value = ToggleLocalPin(value, id);
	}
	Require(int(ParseLocalPins(value).size()) == kLocalPinsLimit,
		"local pins not capped");
	Require(ValidLocalPins(QString()) && ValidLocalPins(value),
		"valid local pins rejected");
	for (const auto bad : { "7,7", "0", "-1", "07", "7,", "a" }) {
		Require(!ValidLocalPins(QString::fromLatin1(bad)),
			"malformed local pins accepted");
	}
	Require(ParseLocalPins(u"7,7"_q).empty(), "malformed local pins used");
}
