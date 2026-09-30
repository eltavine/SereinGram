#pragma once

#include "base/basic_types.h"
#include "serein/core/options.h"
#include "serein/hooks/privacy/alias.h"

namespace Serein::Privacy {

using PeerAliases = base::flat_map<PeerId, QString>;

[[nodiscard]] bool ValidAlias(const QString &value);
[[nodiscard]] std::optional<PeerAliases> ParseAliases(const QByteArray &raw);
[[nodiscard]] QByteArray SerializeAliases(const PeerAliases &aliases);
[[nodiscard]] QString SetAlias(
	not_null<PeerData*> peer,
	const QString &value,
	const QString &expected);

inline const auto kAliases = Option<QByteArray>{
	"serein.peerAliases", Scope::Account, QByteArray(),
	Category::Privacy, "lng_serein_peer_alias", 0,
	[](const QByteArray &raw) { return ParseAliases(raw).has_value(); } };

inline void RegisterAliasOptions(Registry &registry) {
	Expects(registry.Add(kAliases));
}

} // namespace Serein::Privacy
