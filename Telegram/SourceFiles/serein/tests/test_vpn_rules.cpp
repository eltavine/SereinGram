#include "serein/network/vpn_rules.h"
#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestVpnRules() {
	using namespace Serein::Network;
	using Kind = InterfaceKind;
	const auto vpn = [](QString name, QString human, Kind kind) {
		return IsVpnInterface({ name, human, kind });
	};
	Require(vpn(u"utun4"_q, u"utun4"_q, Kind::Unknown), "macOS tunnel");
	Require(vpn(u"tun0"_q, u"tun0"_q, Kind::Virtual), "Linux tunnel");
	Require(vpn(u"tap0"_q, u"tap0"_q, Kind::Physical), "TAP device");
	Require(
		vpn(u"other_32768"_q, u"WireGuard Tunnel"_q, Kind::Unknown),
		"Windows wintun adapter");
	Require(
		vpn(u"ethernet_32777"_q, u"OpenVPN TAP-Windows6"_q, Kind::Physical),
		"Windows TAP adapter");
	Require(
		vpn(u"ppp_32768"_q, u"Office VPN"_q, Kind::Ppp),
		"Windows built-in VPN");
	Require(!vpn(u"en0"_q, u"en0"_q, Kind::Physical), "Wi-Fi");
	Require(
		!vpn(u"ethernet_32768"_q, u"Ethernet"_q, Kind::Physical),
		"Windows Ethernet");
	Require(
		!vpn(u"ppp0"_q, u"Broadband Connection"_q, Kind::Ppp),
		"PPPoE dial-up");
	Require(!vpn(u"wwan0"_q, u"wwan0"_q, Kind::Virtual), "Linux modem");
	Require(
		!vpn(u"mbb_32768"_q, u"Cellular"_q, Kind::Unknown),
		"Windows mobile broadband");
	Require(!vpn(u"lo0"_q, u"lo0"_q, Kind::Loopback), "loopback");
}
