#include "serein/filters/reveal.h"

#include "serein/hooks/display/view_refresher.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "main/main_session.h"

#include <QtCore/QSet>

#include <map>

namespace Serein::Filters {
namespace {

using RevealedChats = std::map<not_null<Main::Session*>, QSet<quint64>>;

[[nodiscard]] RevealedChats &Chats() {
	static auto result = RevealedChats();
	return result;
}

} // namespace

bool Revealed(not_null<::History*> history) {
	const auto &all = Chats();
	const auto i = all.find(&history->session());
	return (i != end(all)) && i->second.contains(history->peer->id.value);
}

void ToggleRevealed(not_null<::History*> history) {
	const auto session = &history->session();
	auto &all = Chats();
	auto i = all.find(session);
	if (i == end(all)) {
		i = all.emplace(session, QSet<quint64>()).first;
		session->lifetime().add([=] {
			Chats().erase(session);
		});
	}
	auto &chats = i->second;
	const auto id = history->peer->id.value;
	if (!chats.remove(id)) {
		chats.insert(id);
	}
	ViewRefresher::Refresh(history->owner());
}

} // namespace Serein::Filters
