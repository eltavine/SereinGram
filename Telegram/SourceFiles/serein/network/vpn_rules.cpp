#include "serein/network/vpn_rules.h"

#include <array>

namespace Serein::Network {
namespace {

constexpr auto kTunnelPrefixes = std::array{
	"utun",
	"tun",
	"tap",
	"wg",
	"ipsec",
	"tailscale",
	"nordlynx",
	"proton",
};

constexpr auto kVpnWords = std::array{
	"vpn",
	"wireguard",
	"wintun",
	"tap-windows",
	"openvpn",
	"tailscale",
	"zerotier",
	"clash",
	"sing-box",
	"v2ray",
	"xray",
	"hiddify",
	"nekoray",
	"warp",
	"anyconnect",
	"globalprotect",
	"forti",
};

constexpr auto kCellularPrefixes = std::array{
	"wwan",
	"rmnet",
	"ww",
};

constexpr auto kCellularWords = std::array{
	"cellular",
	"mobile broadband",
};

template <size_t Size>
[[nodiscard]] bool StartsWithAny(
		const QString &value,
		const std::array<const char*, Size> &list) {
	for (const auto prefix : list) {
		if (value.startsWith(QLatin1String(prefix))) {
			return true;
		}
	}
	return false;
}

template <size_t Size>
[[nodiscard]] bool ContainsAny(
		const QString &value,
		const std::array<const char*, Size> &list) {
	for (const auto word : list) {
		if (value.contains(QLatin1String(word))) {
			return true;
		}
	}
	return false;
}

} // namespace

bool IsVpnInterface(const InterfaceInfo &info) {
	const auto name = info.name.toLower();
	const auto human = info.humanName.toLower();
	if (info.kind == InterfaceKind::Loopback
		|| StartsWithAny(name, kCellularPrefixes)
		|| ContainsAny(human, kCellularWords)) {
		return false;
	} else if (info.kind == InterfaceKind::Ppp) {
		return ContainsAny(human, kVpnWords);
	} else if (info.kind == InterfaceKind::Virtual
		|| info.kind == InterfaceKind::Unknown) {
		return true;
	}
	return StartsWithAny(name, kTunnelPrefixes)
		|| StartsWithAny(human, kTunnelPrefixes)
		|| ContainsAny(human, kVpnWords);
}

} // namespace Serein::Network
