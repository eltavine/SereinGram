#include "serein/privacy/alias.h"

#include "serein/schema/gen/config/aliases.h"
#include "data/data_peer_id.h"

namespace Serein::Privacy {
namespace {

constexpr auto kMaximumRawSize = 512 * 1024;

[[nodiscard]] std::optional<PeerId> AliasPeer(const QString &key) {
	auto ok = false;
	const auto id = DeserializePeerId(key.toULongLong(&ok));
	if (!ok || !id
		|| (!peerIsUser(id) && !peerIsChat(id) && !peerIsChannel(id))
		|| peerToBareMTPInt(id).v <= 0
		|| QString::number(SerializePeerId(id)) != key) {
		return std::nullopt;
	}
	return id;
}

} // namespace

std::optional<PeerAliases> ParseAliases(const QByteArray &raw) {
	if (raw.isEmpty()) {
		return PeerAliases();
	} else if (raw.size() > kMaximumRawSize) {
		return std::nullopt;
	}
	const auto config = ParsePeerAliasesConfig(raw);
	if (!config) {
		return std::nullopt;
	}
	auto result = PeerAliases();
	for (const auto &[key, value] : config->names) {
		const auto peer = AliasPeer(key);
		if (!peer) {
			return std::nullopt;
		}
		result.emplace(*peer, value);
	}
	return result;
}

QByteArray SerializeAliases(const PeerAliases &aliases) {
	if (aliases.empty()) {
		return {};
	}
	auto config = PeerAliasesConfig();
	for (const auto &[id, value] : aliases) {
		config.names.emplace(QString::number(SerializePeerId(id)), value);
	}
	return SerializePeerAliasesConfig(config);
}

} // namespace Serein::Privacy
