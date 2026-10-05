#pragma once

#include "data/data_msg_id.h"
#include "history/history_item.h"
#include "serein/ports/history_store.h"

class History;

namespace Serein::HistoryFeature {

struct RestoreArgs {
	MsgId id;
	MessageFlags flags;
	TimeId date = 0;
	bool asLogEntry = false;
};

[[nodiscard]] not_null<HistoryItem*> MakeRestoredMessage(
	not_null<::History*> history,
	const History::Record &record,
	RestoreArgs args);

} // namespace Serein::HistoryFeature
