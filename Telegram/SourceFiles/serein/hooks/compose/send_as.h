#pragma once

#include "serein/hooks/gen/compose.h"

#include <algorithm>
#include <type_traits>

namespace Serein::Compose {

template <typename Key, typename List>
void AddOwnerSendAs(const Key &key, List &list) {
	using Type = std::remove_cvref_t<decltype(key.type)>;
	const auto channel = key.peer->asMegagroup();
	if (key.type != Type::Message
		|| !channel
		|| !channel->amCreator()
		|| !Hooks::Compose::OwnerSendAs()) {
		return;
	}
	const auto add = [&](auto peer) {
		const auto known = std::any_of(list.begin(), list.end(), [&](
				const auto &entry) {
			return entry.peer.get() == peer;
		});
		if (!known) {
			list.insert(
				list.begin() + (list.empty() ? 0 : 1),
				typename List::value_type{ .peer = peer });
		}
	};
	add(channel);
	add(channel->session().user().get());
}

} // namespace Serein::Compose
