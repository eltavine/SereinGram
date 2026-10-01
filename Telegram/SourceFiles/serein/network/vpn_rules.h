#pragma once

#include <QtCore/QString>

namespace Serein::Network {

enum class InterfaceKind {
	Physical,
	Virtual,
	Ppp,
	Loopback,
	Unknown,
};

struct InterfaceInfo {
	QString name;
	QString humanName;
	InterfaceKind kind = InterfaceKind::Unknown;
};

// The interface is the one the system routes Telegram traffic through.
[[nodiscard]] bool IsVpnInterface(const InterfaceInfo &info);

} // namespace Serein::Network
