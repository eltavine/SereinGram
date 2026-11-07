#pragma once

#include "serein/ports/history_store.h"
#include "history/history_item.h"

#include <map>
#include <memory>
#include <optional>
#include <vector>

class History;

namespace Serein::HistoryFeature::Viewer {

struct ItemEntry {
	History::Record record;
	TimeId date = 0;
	bool version = false;
	bool deleted = false;
	bool edited = false;
};

class RestoredItems final {
public:
	explicit RestoredItems(not_null<::History*> history);
	RestoredItems(const RestoredItems &) = delete;
	RestoredItems &operator=(const RestoredItems &) = delete;
	~RestoredItems();

	[[nodiscard]] not_null<::History*> history() const;
	[[nodiscard]] not_null<HistoryItem*> ensure(const ItemEntry &entry);
	[[nodiscard]] std::optional<Ports::RecordKey> keyOf(FullMsgId id) const;
	void retain(const std::vector<Ports::RecordKey> &keys);

private:
	using Owned = std::unique_ptr<HistoryItem, HistoryItem::Destroyer>;

	void destroy(std::map<Ports::RecordKey, Owned>::iterator i);

	const not_null<::History*> _history;
	std::map<Ports::RecordKey, Owned> _items;
	base::flat_map<FullMsgId, Ports::RecordKey> _keys;
	rpl::lifetime _lifetime;

};

} // namespace Serein::HistoryFeature::Viewer
