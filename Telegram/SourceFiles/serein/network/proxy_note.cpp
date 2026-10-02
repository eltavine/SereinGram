#include "serein/hooks/network/proxy_note.h"

#include "serein/core/options.h"
#include "serein/network/proxy_notes.h"
#include "serein/schema/gen/settings/services.h"

namespace Serein {

TextWithEntities Hooks::Network::ProxyNote(const QString &host, uint32 port) {
	const auto notes = Serein::Network::ParseProxyNotes(
		ForDevice().Get(Serein::ServiceSettings::kProxyNotes));
	if (!notes) {
		return {};
	}
	const auto i = notes->find(Serein::Network::ProxyNoteKey(host, port));
	return (i != notes->end())
		? TextWithEntities{ u" \u00B7 "_q + i->second }
		: TextWithEntities();
}

} // namespace Serein
