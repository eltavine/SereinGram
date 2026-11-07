#include "serein/features/history/viewer/items.h"

#include "serein/features/history/copies.h"
#include "serein/features/history/restored_message.h"
#include "data/data_session.h"
#include "history/history.h"

#include <algorithm>

namespace Serein::HistoryFeature::Viewer {

RestoredItems::RestoredItems(not_null<::History*> history)
: _history(history) {
	_history->owner().itemRemoved(
	) | rpl::on_next([=](not_null<const HistoryItem*> item) {
		const auto i = _keys.find(item->fullId());
		if (i == end(_keys)) {
			return;
		}
		const auto j = _items.find(i->second);
		_keys.erase(i);
		UnregisterCopy(item);
		if (j != end(_items)) {
			j->second.release();
			_items.erase(j);
		}
	}, _lifetime);
}

RestoredItems::~RestoredItems() {
	_lifetime.destroy();
	while (!_items.empty()) {
		destroy(begin(_items));
	}
}

not_null<::History*> RestoredItems::history() const {
	return _history;
}

not_null<HistoryItem*> RestoredItems::ensure(const ItemEntry &entry) {
	const auto key = Ports::KeyOf(entry.record);
	if (const auto i = _items.find(key); i != end(_items)) {
		return i->second.get();
	}
	const auto item = MakeRestoredMessage(_history, entry.record, {
		.id = _history->nextNonHistoryEntryId(),
		.date = entry.date,
		.asLogEntry = true,
	});
	RegisterCopy(item, {
		.record = entry.record,
		.version = entry.version,
		.deleted = entry.deleted,
		.edited = entry.edited,
	});
	_keys.emplace(item->fullId(), key);
	_items.emplace(key, Owned(item.get()));
	return item;
}

std::optional<Ports::RecordKey> RestoredItems::keyOf(FullMsgId id) const {
	const auto i = _keys.find(id);
	return (i != end(_keys))
		? std::make_optional(i->second)
		: std::nullopt;
}

void RestoredItems::retain(const std::vector<Ports::RecordKey> &keys) {
	for (auto i = begin(_items); i != end(_items);) {
		if (std::find(begin(keys), end(keys), i->first) != end(keys)) {
			++i;
		} else {
			const auto next = std::next(i);
			destroy(i);
			i = next;
		}
	}
}

void RestoredItems::destroy(std::map<Ports::RecordKey, Owned>::iterator i) {
	auto owned = std::move(i->second);
	_items.erase(i);
	_keys.remove(owned->fullId());
	UnregisterCopy(owned.get());
	owned = nullptr;
}

} // namespace Serein::HistoryFeature::Viewer
